/*
 * Copyright (c) 2017, Weptech elektronik GmbH Germany
 * http://www.weptech.de
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */
/*---------------------------------------------------------------------------*/
/**
 * \addtogroup cc26xx-platforms
 * @{
 *
 * \defgroup coua Coua
 *
 *  The Coua is a minimalistic dev board that showcases Weptech elektronik's
 *  6LoWPAN modules. Featuring a JTAG interface, breakout headers and an
 *  optional SMA antenna jack, it can be used to kickstart your development of
 *  6LoWPAN devices.
 * @{
 */
#include "ti-lib.h"
#include "contiki.h"
#include "contiki-net.h"

#include "lpm.h"
#include "gpio-interrupt.h"
#include "ieee-addr.h"
#include "vims.h"
#include "uart.h"
#include "sys_ctrl.h"
#include "sys/clock.h"
#include "sys/rtimer.h"
#include "sys/node-id.h"
#include "lib/random.h"
#include "lib/sensors.h"
#include "dev/watchdog.h"
#include "dev/oscillators.h"
#include "dev/serial-line.h"
#include "dev/cc26xx-uart.h"
#include "dev/soc-rtc.h"
#include "rf-core/rf-core.h"
#include "net/mac/frame802154.h"

#include "driverlib/driverlib_release.h"

#include <stdio.h>
/*---------------------------------------------------------------------------*/
unsigned short node_id = 0;
/*---------------------------------------------------------------------------*/
static void
set_rf_params()
{
  uint16_t short_addr;
  uint8_t ext_addr[8];
  radio_value_t val = 0;

  ieee_addr_cpy_to(ext_addr, 8);

  short_addr = ext_addr[7];
  short_addr |= ext_addr[6] << 8;

  /* Populate linkaddr_node_addr. Maintain endianness */
  memcpy(&linkaddr_node_addr, &ext_addr[8 - LINKADDR_SIZE], LINKADDR_SIZE);

  NETSTACK_RADIO.set_value(RADIO_PARAM_PAN_ID, IEEE802154_PANID);
  NETSTACK_RADIO.set_value(RADIO_PARAM_16BIT_ADDR, short_addr);
  NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, RF_CORE_CHANNEL);
  NETSTACK_RADIO.set_object(RADIO_PARAM_64BIT_ADDR, ext_addr, 8);

  NETSTACK_RADIO.get_value(RADIO_PARAM_CHANNEL, &val);
  printf(" RF: Channel %d\n", val);

#if STARTUP_CONF_VERBOSE
  {
    int i;
    printf(" Link layer addr: ");
    for(i = 0; i < LINKADDR_SIZE - 1; i++) {
      printf("%02x:", linkaddr_node_addr.u8[i]);
    }
    printf("%02x\n", linkaddr_node_addr.u8[i]);
  }
#endif

  /* also set the global node id */
  node_id = short_addr;
  printf(" Node ID: %d\n", node_id);
}
/*---------------------------------------------------------------------------*/
static void
wakeup_handler(void)
{
  /* Turn on the PERIPH PD */
  ti_lib_prcm_power_domain_on(PRCM_DOMAIN_PERIPH);
  while((ti_lib_prcm_power_domain_status(PRCM_DOMAIN_PERIPH)
        != PRCM_DOMAIN_POWER_ON));
}
/*---------------------------------------------------------------------------*/
/*
 * Declare a data structure to register with LPM.
 * We don't care about what power mode we'll drop to, we don't care about
 * getting notified before deep sleep. All we need is to be notified when we
 * wake up so we can turn power domains back on
 */
LPM_MODULE(coua_lpm_module, NULL, NULL, wakeup_handler, LPM_DOMAIN_NONE);
/*---------------------------------------------------------------------------*/
void
board_init()
{
  /* Disable global interrupts */
  bool int_disabled = ti_lib_int_master_disable();

  wakeup_handler();

  /* Enable GPIO peripheral */
  ti_lib_prcm_peripheral_run_enable(PRCM_PERIPH_GPIO);

  /* Apply settings and wait for them to take effect */
  ti_lib_prcm_load_set();
  while(!ti_lib_prcm_load_get());

  lpm_register_module(&coua_lpm_module);

  /* Re-enable interrupt if initially enabled. */
  if(!int_disabled) {
    ti_lib_int_master_enable();
  }
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Main function for the Coua
 */
int
main(void)
{
  /* Enable flash cache and prefetch. */
  ti_lib_vims_mode_set(VIMS_BASE, VIMS_MODE_ENABLED);
  ti_lib_vims_configure(VIMS_BASE, true, true);

  ti_lib_int_master_disable();

  /* Set the LF XOSC as the LF system clock source */
  oscillators_select_lf_xosc();

  lpm_init();

  board_init();

  gpio_interrupt_init();

  /*
   * Disable I/O pad sleep mode and open I/O latches in the AON IOC interface
   * This is only relevant when returning from shutdown (which is what froze
   * latches in the first place. Before doing these things though, we should
   * allow software to first regain control of pins
   */
  ti_lib_pwr_ctrl_io_freeze_disable();

  ti_lib_int_master_enable();

  soc_rtc_init();
  clock_init();
  rtimer_init();

  watchdog_init();
  process_init();

  random_init(0x1234);

  /* Character I/O Initialisation */
#if CC26XX_UART_CONF_ENABLE
  cc26xx_uart_init();
#endif

  serial_line_init();

  printf("Starting " CONTIKI_VERSION_STRING "\n");
  printf("With DriverLib v%u.%u\n", DRIVERLIB_RELEASE_GROUP,
         DRIVERLIB_RELEASE_BUILD);
  printf(BOARD_STRING "\n");
  printf("IEEE 802.15.4: %s, Sub-GHz: %s, BLE: %s, Prop: %s\n",
         ti_lib_chipinfo_supports_ieee_802_15_4() == true ? "Yes" : "No",
         ti_lib_chipinfo_chip_family_is_cc13xx() == true ? "Yes" : "No",
         ti_lib_chipinfo_supports_ble() == true ? "Yes" : "No",
         ti_lib_chipinfo_supports_proprietary() == true ? "Yes" : "No");


  process_start(&etimer_process, NULL);
  ctimer_init();

  energest_init();
  ENERGEST_ON(ENERGEST_TYPE_CPU);

  printf(" Net: ");
  printf("%s\n", NETSTACK_NETWORK.name);
  printf(" MAC: ");
  printf("%s\n", NETSTACK_MAC.name);
  printf(" RDC: ");
  printf("%s", NETSTACK_RDC.name);

  if(NETSTACK_RDC.channel_check_interval() != 0) {
    printf(", Channel Check Interval: %u ticks",
           NETSTACK_RDC.channel_check_interval());
  }
  printf("\n");

  netstack_init();
  set_rf_params();

#if NETSTACK_CONF_WITH_IPV6
  memcpy(&uip_lladdr.addr, &linkaddr_node_addr, sizeof(uip_lladdr.addr));
  queuebuf_init();
  process_start(&tcpip_process, NULL);
#endif /* NETSTACK_CONF_WITH_IPV6 */

  leds_init();
  process_start(&sensors_process, NULL);

  autostart_start(autostart_processes);

  watchdog_start();

  while(1) {
    uint8_t r;
    do {
      r = process_run();
      watchdog_periodic();
    } while(r > 0);

    /* Drop to some low power mode */
    lpm_drop();
  }
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 */
