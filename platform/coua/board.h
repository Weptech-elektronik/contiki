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
 *
 * @{
 *
 * \defgroup coua-board Board Definitions
 *
 * This file provides connectivity information on LEDs, UARTs, etc.
 *
 * @{
 *
 * \file
 *      Header file with definitions related to the I/O connections
 *
 * \note Do not include this file directly. It gets included by contiki-conf.h
 *       after all relevant directives have been set.
 */

#ifndef BOARD_H_
#define BOARD_H_
/*---------------------------------------------------------------------------*/
/**
 * \name LED configuration
 * @{
 */
/*---------------------------------------------------------------------------*/
/* Some files include leds.h before us, so we need to get rid of defaults in
 * leds.h before we provide correct definitions */
#undef LEDS_GREEN
#undef LEDS_YELLOW
#undef LEDS_RED
#undef LEDS_CONF_ALL


/* Notify various examples that we have no LEDs */
#define PLATFORM_HAS_LEDS               0
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name UART configuration
 *
 * @{
 */
#define BOARD_IOID_UART_RX                      IOID_1
#define BOARD_IOID_UART_TX                      IOID_2
#define BOARD_IOID_UART_RTS                     IOID_0
#define BOARD_IOID_UART_CTS                     IOID_5
#define BOARD_UART_RX             (1 << BOARD_IOID_UART_RX)
#define BOARD_UART_TX             (1 << BOARD_IOID_UART_TX)
#define BOARD_UART_RTS            (1 << BOARD_IOID_UART_RTS)
#define BOARD_UART_CTS            (1 << BOARD_IOID_UART_CTS)
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name Bootloader configuration
 *
 * @{
 */
#if ROM_BOOTLOADER_ENABLE
#define SET_CCFG_BL_CONFIG_BOOTLOADER_ENABLE            0xC5
#define SET_CCFG_BL_CONFIG_BL_LEVEL                     0x00
#define SET_CCFG_BL_CONFIG_BL_PIN_NUMBER                IOID_9
#define SET_CCFG_BL_CONFIG_BL_ENABLE                    0xC5
#else
#define SET_CCFG_BL_CONFIG_BOOTLOADER_ENABLE            0x00
#define SET_CCFG_BL_CONFIG_BL_LEVEL                     0x01
#define SET_CCFG_BL_CONFIG_BL_PIN_NUMBER                0xFF
#define SET_CCFG_BL_CONFIG_BL_ENABLE                    0xFF
#endif
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name Device string used on startup
 * @{
 */
#define BOARD_STRING                    "Coua"
/** @} */
/*---------------------------------------------------------------------------*/
#endif /* BOARD_H_ */

/**
 * @}
 * @}
 */
