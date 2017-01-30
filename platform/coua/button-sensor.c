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
 * \addtogroup coua-button-sensor
 * @{
 *
 * \file
 *      Driver for Coua buttons
 */
/*---------------------------------------------------------------------------*/
#include "contiki.h"
#include "lib/sensors.h"
#include "dev/button-sensor.h"
#include "gpio-interrupt.h"
#include "sys/timer.h"
#include "lpm.h"

#include "ti-lib.h"

#include <stdint.h>
/*---------------------------------------------------------------------------*/
#define BUTTON_GPIO_CFG         (IOC_CURRENT_2MA  | IOC_STRENGTH_AUTO | \
                                 IOC_IOPULL_UP    | IOC_SLEW_DISABLE  | \
                                 IOC_HYST_DISABLE | IOC_BOTH_EDGES    | \
                                 IOC_INT_ENABLE   | IOC_IOMODE_NORMAL | \
                                 IOC_NO_WAKE_UP   | IOC_INPUT_ENABLE)
/*---------------------------------------------------------------------------*/
#define DEBOUNCE_DURATION (CLOCK_SECOND >> 5)

struct btn_timer {
  struct timer debounce;
  clock_time_t start;
  clock_time_t duration;
};

static struct btn_timer timer;
/*---------------------------------------------------------------------------*/
static void
button_press_handler()
{
    if(!timer_expired(&timer.debounce)) {
      return;
    }

    timer_set(&timer.debounce, DEBOUNCE_DURATION);

    /*
     * Start press duration counter on press (falling), notify on release
     * (rising)
     */
    if(ti_lib_gpio_read_dio(BOARD_IOID_KEY) == 0) {
      timer.start = clock_time();
      timer.duration = 0;
    } else {
      timer.duration = clock_time() - timer.start;
      sensors_changed(&button_sensor);
    }
}
/*---------------------------------------------------------------------------*/
int
config(int type, int c)
{
  switch(type) {
  case SENSORS_HW_INIT:
    ti_lib_gpio_clear_event_dio(BOARD_IOID_KEY);
    ti_lib_rom_ioc_pin_type_gpio_input(BOARD_IOID_KEY);
    ti_lib_rom_ioc_port_configure_set(BOARD_IOID_KEY, IOC_PORT_GPIO, BUTTON_GPIO_CFG);
    gpio_interrupt_register_handler(BOARD_IOID_KEY, button_press_handler);
    break;
  case SENSORS_ACTIVE:
    if(c) {
      ti_lib_gpio_clear_event_dio(BOARD_IOID_KEY);
      ti_lib_rom_ioc_pin_type_gpio_input(BOARD_IOID_KEY);
      ti_lib_rom_ioc_port_configure_set(BOARD_IOID_KEY, IOC_PORT_GPIO, BUTTON_GPIO_CFG);
      ti_lib_rom_ioc_int_enable(BOARD_IOID_KEY);
    } else {
      ti_lib_rom_ioc_int_disable(BOARD_IOID_KEY);
    }
    break;
  default:
    break;
  }

  return 1;
}
/*---------------------------------------------------------------------------*/
int
status(int type)
{
  switch(type) {
  case SENSORS_ACTIVE:
  case SENSORS_READY:
    if(ti_lib_rom_ioc_port_configure_get(BOARD_IOID_KEY) & IOC_INT_ENABLE) {
      return 1;
    }
    break;
  default:
    break;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
int
value(int type)
{
    return ti_lib_gpio_read_dio(BOARD_IOID_KEY) == 0 ? 1 : 0;
}
/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(button_sensor, BUTTON_SENSOR, value, config, status);
/*---------------------------------------------------------------------------*/
/** @} */
