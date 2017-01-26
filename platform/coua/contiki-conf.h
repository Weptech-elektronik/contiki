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

/**
 * \addtogroup coua
 * @{
 *
 * \file
 *      Configuration for the Coua platform
 */
#ifndef CONTIKI_CONF_H
#define CONTIKI_CONF_H

#include <stdint.h>

/*---------------------------------------------------------------------------*/
/* Include project specific configuration file */
#ifdef PROJECT_CONF_H
#include PROJECT_CONF_H
#endif /* PROJECT_CONF_H */
/*---------------------------------------------------------------------------*/
/**
 * \name Compiler configuration and platform-specific type definitions
 *
 * Those values are not meant to be modified by the user
 * @{
 */
#define CLOCK_CONF_SECOND 128

/* Compiler configurations */
#define CCIF
#define CLIF

/* Platform typedefs */
typedef uint32_t clock_time_t;
typedef uint32_t uip_stats_t;

/*
 * rtimer.h typedefs rtimer_clock_t as unsigned short. We need to define
 * RTIMER_CLOCK_DIFF to override this
 */
typedef uint32_t rtimer_clock_t;
#define RTIMER_CLOCK_DIFF(a, b)     ((int32_t)((a) - (b)))
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name ROM Bootloader configuration
 *
 * Per se, the board supports the serial bootloader, but has no buttons to boot
 * the CPU into bootloader mode. You can, however, solder on pin headers and use
 * a jumper to indicate when you want to boot into bootloader mode. Then, set
 * this to 1 and configure your jumper in board.h.
 *
 * @{
 */
#ifndef ROM_BOOTLOADER_ENABLE
#define ROM_BOOTLOADER_ENABLE              1
#endif
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name Crystal configuration
 * @{
 */
/**
 * This value is used to mitigate load capacitances of the external 24MHz
 * crystal that is used by the radio core.
 */
#define SET_CCFG_MODE_CONF_XOSC_CAPARRAY_DELTA  0xF4
#define SET_CCFG_MODE_CONF_XOSC_CAP_MOD         0
/**
 * If this is set, the systems keeps the HF crystal oscillator on even when the
 * radio is off, reducing the radio startup time.
 */
#ifndef CC2650_FAST_RADIO_STARTUP
#define CC2650_FAST_RADIO_STARTUP               0
#endif
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name Generic Configuration directives
 *
 * @{
 */
#ifndef ENERGEST_CONF_ON
#define ENERGEST_CONF_ON            0 /**< Energest Module */
#endif

#ifndef STARTUP_CONF_VERBOSE
#define STARTUP_CONF_VERBOSE        1 /**< Set to 0 to decrease startup verbosity */
#endif
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name Character I/O Configuration
 *
 * @{
 */
#ifndef CC26XX_UART_CONF_ENABLE
#define CC26XX_UART_CONF_ENABLE            1 /**< Enable/Disable UART I/O */
#endif

#ifndef CC26XX_UART_CONF_BAUD_RATE
#define CC26XX_UART_CONF_BAUD_RATE    115200 /**< Default UART0 baud rate */
#endif

/* Turn off example-provided putchars */
#define SLIP_BRIDGE_CONF_NO_PUTCHAR        1
#define SLIP_RADIO_CONF_NO_PUTCHAR         1

#ifndef SLIP_ARCH_CONF_ENABLED
/*
 * Determine whether we need SLIP
 * This will keep working while UIP_FALLBACK_INTERFACE and CMD_CONF_OUTPUT
 * keep using SLIP
 */
#if defined(UIP_FALLBACK_INTERFACE) || defined(CMD_CONF_OUTPUT)
#define SLIP_ARCH_CONF_ENABLED             1
#endif
#endif
/** @} */
/*---------------------------------------------------------------------------*/
/* board.h assumes that basic configuration is done */
#include "board.h"
/*---------------------------------------------------------------------------*/
/**
 * \name Radio driver selection
 *
 * Auto-configure Prop-mode radio unless the project has specified otherwise.
 * @{
 */
#ifndef CC13XX_CONF_PROP_MODE
#define CC13XX_CONF_PROP_MODE 1
#endif /* CC13XX_CONF_PROP_MODE */

#if CC13XX_CONF_PROP_MODE
#define NETSTACK_CONF_RADIO                     prop_mode_driver
#else
#define NETSTACK_CONF_RADIO                     ieee_mode_driver
#endif

#ifndef IEEE_MODE_CONF_PROMISCOUS
#define IEEE_MODE_CONF_PROMISCOUS       0 /**< 1 to enable promiscous mode for the ieee driver. */
#endif
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name Network Stack Configuration
 *
 * @{
 */
#ifndef IEEE802154_CONF_PANID
#define IEEE802154_CONF_PANID           0xABCD /**< Default PAN ID */
#endif

#ifndef RF_CORE_CONF_CHANNEL
#define RF_CORE_CONF_CHANNEL            25
#endif

#ifndef NETSTACK_CONF_NETWORK
#if NETSTACK_CONF_WITH_IPV6
#define NETSTACK_CONF_NETWORK           sicslowpan_driver
#else
#define NETSTACK_CONF_NETWORK           rime_driver
#endif /* NETSTACK_CONF_WITH_IPV6 */
#endif /* NETSTACK_CONF_NETWORK */

#ifndef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC               csma_driver
#endif

#ifndef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC               contikimac_driver
#endif

#ifndef NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE
#define NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE 8
#endif

#ifndef NETSTACK_CONF_FRAMER
#if NETSTACK_CONF_WITH_IPV6
#define NETSTACK_CONF_FRAMER            framer_802154
#else /* NETSTACK_CONF_WITH_IPV6 */
#define NETSTACK_CONF_FRAMER            contikimac_framer
#endif /* NETSTACK_CONF_WITH_IPV6 */
#endif /* NETSTACK_CONF_FRAMER */

/*---------------------------------------------------------------------------*/
/* Configure NullRDC for when it's selected */
/* Automatically await others people's acks */
#define NULLRDC_CONF_802154_AUTOACK          1

#if CC13XX_CONF_PROP_MODE
#define NULLRDC_CONF_ACK_WAIT_TIME           (RTIMER_SECOND / 400)
#define NULLRDC_CONF_AFTER_ACK_DETECTED_WAIT_TIME (RTIMER_SECOND / 1000)
#define NULLRDC_CONF_802154_AUTOACK_HW       0
#define NULLRDC_CONF_SEND_802154_ACK         1
#else
#define NULLRDC_CONF_802154_AUTOACK_HW       1
#define NULLRDC_CONF_SEND_802154_ACK         0
#ifndef IEEE_MODE_CONF_AUTOACK
#define IEEE_MODE_CONF_AUTOACK               1 /**< RF H/W generates ACKs */
#endif
#endif
/*---------------------------------------------------------------------------*/
/* Configure ContikiMAC for when it's selected */
#define CONTIKIMAC_CONF_WITH_CONTIKIMAC_HEADER  0
#define CONTIKIMAC_CONF_WITH_PHASE_OPTIMIZATION 0
#define WITH_FAST_SLEEP                         1

#if CC13XX_CONF_PROP_MODE

/* Default in contikimac.c is fine, just listed for documentation. */
/* #ifndef CONTIKIMAC_CONF_CYCLE_TIME */
/* #define CONTIKIMAC_CONF_CYCLE_TIME              RTIMER_ARCH_SECOND/8 */
/* #endif *//* CONTIKIMAC_CONF_CYCLE_TIME */

#ifndef CONTIKIMAC_CONF_CCA_CHECK_TIME
#define CONTIKIMAC_CONF_CCA_CHECK_TIME          RTIMER_ARCH_SECOND/1000
#endif /* CONTIKIMAC_CONF_CCA_CHECK_TIME */

#ifndef CONTIKIMAC_CONF_CCA_SLEEP_TIME
#define CONTIKIMAC_CONF_CCA_SLEEP_TIME          RTIMER_ARCH_SECOND/140
#endif /* CONTIKIMAC_CONF_CCA_SLEEP_TIME */

#ifndef CONTIKIMAC_CONF_LISTEN_TIME_AFTER_PACKET_DETECTED
#define CONTIKIMAC_CONF_LISTEN_TIME_AFTER_PACKET_DETECTED  RTIMER_ARCH_SECOND/20
#endif /* CONTIKIMAC_CONF_LISTEN_TIME_AFTER_PACKET_DETECTED */

#ifndef CONTIKIMAC_CONF_SEND_SW_ACK
#define CONTIKIMAC_CONF_SEND_SW_ACK             1
#endif /* CONTIKIMAC_CONF_SEND_SW_ACK */

#ifndef CONTIKIMAC_CONF_AFTER_ACK_DETECTED_WAIT_TIME
#define CONTIKIMAC_CONF_AFTER_ACK_DETECTED_WAIT_TIME RTIMER_ARCH_SECOND/400
#endif /* CONTIKIMAC_CONF_AFTER_ACK_DETECTED_WAIT_TIME */

#ifndef CONTIKIMAC_CONF_INTER_PACKET_INTERVAL
#define CONTIKIMAC_CONF_INTER_PACKET_INTERVAL   RTIMER_ARCH_SECOND/150
#endif /* CONTIKIMAC_CONF_INTER_PACKET_INTERVAL */

#ifndef CONTIKIMAC_CONF_INTER_PACKET_DEADLINE
#define CONTIKIMAC_CONF_INTER_PACKET_DEADLINE   RTIMER_ARCH_SECOND/20
#endif /* CONTIKIMAC_CONF_INTER_PACKET_DEADLINE */

#ifndef CONTIKIMAC_CONF_GUARD_TIME
#define CONTIKIMAC_CONF_GUARD_TIME              RTIMER_ARCH_SECOND/12
#endif /* CONTIKIMAC_CONF_GUARD_TIME */

#ifndef CONTIKIMAC_CONF_MAX_PHASE_STROBE_TIME
#define CONTIKIMAC_CONF_MAX_PHASE_STROBE_TIME   RTIMER_ARCH_SECOND/10
#endif /* CONTIKIMAC_CONF_MAX_PHASE_STROBE_TIME */
#else
#endif

/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name IEEE address configuration
 *
 * Used to generate our RIME & IPv6 address
 * @{
 */
/**
 * \brief Location of the IEEE address
 * 0 => Read from InfoPage,
 * 1 => Use a hardcoded address, configured by IEEE_ADDR_CONF_ADDRESS
 */
#ifndef IEEE_ADDR_CONF_HARDCODED
#define IEEE_ADDR_CONF_HARDCODED             0
#endif

/**
 * \brief The hardcoded IEEE address to be used when IEEE_ADDR_CONF_HARDCODED
 * is defined as 1
 */
#ifndef IEEE_ADDR_CONF_ADDRESS
#define IEEE_ADDR_CONF_ADDRESS { 0x00, 0x12, 0x4B, 0x00, 0x89, 0xAB, 0xCD, 0xEF }
#endif
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name IPv6, RIME and network buffer configuration
 *
 * @{
 */
/* Don't let contiki-default-conf.h decide if we are an IPv6 build */
#ifndef NETSTACK_CONF_WITH_IPV6
#define NETSTACK_CONF_WITH_IPV6              0
#endif

#if NETSTACK_CONF_WITH_IPV6
/*---------------------------------------------------------------------------*/
/* Addresses, Sizes and Interfaces */
#define LINKADDR_CONF_SIZE                   8
#define UIP_CONF_LL_802154                   1
#define UIP_CONF_LLH_LEN                     0

/* The size of the uIP main buffer */
#ifndef UIP_CONF_BUFFER_SIZE
#define UIP_CONF_BUFFER_SIZE              1000
#endif

/* ND and Routing */
#ifndef UIP_CONF_ROUTER
#define UIP_CONF_ROUTER                      1
#endif

#define UIP_CONF_ND6_SEND_RA                 0
#define UIP_CONF_IP_FORWARD                  0
#define RPL_CONF_STATS                       0

#define UIP_CONF_ND6_REACHABLE_TIME     600000
#define UIP_CONF_ND6_RETRANS_TIMER       10000

#ifndef NBR_TABLE_CONF_MAX_NEIGHBORS
#define NBR_TABLE_CONF_MAX_NEIGHBORS        20
#endif
#ifndef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES                 20
#endif

#ifndef UIP_CONF_TCP
#define UIP_CONF_TCP                         1
#endif
#ifndef UIP_CONF_TCP_MSS
#define UIP_CONF_TCP_MSS                    64
#endif

#define UIP_CONF_UDP                         1
#define UIP_CONF_UDP_CHECKSUMS               1
#define UIP_CONF_ICMP6                       1

/* 6LoWPAN */
#define SICSLOWPAN_CONF_COMPRESSION             SICSLOWPAN_COMPRESSION_HC06
#define SICSLOWPAN_CONF_COMPRESSION_THRESHOLD   63
#define SICSLOWPAN_CONF_FRAG                    1
#define SICSLOWPAN_CONF_MAXAGE                  8
/*---------------------------------------------------------------------------*/
#else /* NETSTACK_CONF_WITH_IPV6 */
/* Network setup for non-IPv6 (rime). */
#define UIP_CONF_IP_FORWARD                  1

#define RIME_CONF_NO_POLITE_ANNOUCEMENTS     0

#endif /* NETSTACK_CONF_WITH_IPV6 */
/** @} */
/*---------------------------------------------------------------------------*/
#endif /* CONTIKI_CONF_H */

/** @} */
