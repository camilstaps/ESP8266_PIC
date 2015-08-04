/**
 * C library for the ESP8266 WiFi module with a PIC microcontroller
 * Copyright (C) 2015 Camil Staps <info@camilstaps.nl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *****************************************************************************
 *
 * File:    esp8266.h
 * Author:  Camil Staps <info@camilstaps.nl>
 * Website: http://github.com/camilstaps/ESP8266_PIC
 * Version: 0.1
 *
 * See:     esp8266.c
 *
 * This is the header file for the ESP8266 PIC18 library. It contains:
 *
 *  * Constants and bitmasks
 *  * Prototypes for functions that should be written by the user for his
 *    implementation
 *  * Prototypes for functions that can be used by the user
 *  * Prototypes for functions that are intended for internal use only
 */

#include <p18f2550.h>
#include <delays.h>
#include <portb.h>
#include <usart.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef ESP8266_H
#define	ESP8266_H

#ifdef	__cplusplus
extern "C" {
#endif

/** Some constants **/

#define ESP8266_STATION 0x01
#define ESP8266_SOFTAP 0x02

#define ESP8266_TCP 1
#define ESP8266_UDP 0

#define ESP8266_OK 1
#define ESP8266_READY 2
#define ESP8266_FAIL 3
#define ESP8266_NOCHANGE 4
#define ESP8266_LINKED 5
#define ESP8266_UNLINK 6
#define ESP8266_ERROR 7
#define ESP8266_NO_IP 8
#define ESP8266_TYPE_ERROR 9
#define ESP8266_SEND_OK 10
#define ESP8266_NO_LINK 11
#define ESP8266_TIMEOUT 255

/** Should be witten by the user for input from / output to the ESP module **/

// Print a string to the output
void _esp8266_print(unsigned const char *);
void _esp8266_putch(unsigned char);
unsigned char _esp8266_getch(void);

/** Function prototypes **/

unsigned char esp8266_isStarted(void);        // Check if the module is started (AT)
unsigned char esp8266_restart(void);          // Restart module (AT+RST)
void esp8266_echoCmds(unsigned char);        // Enabled/disable command echoing (ATE)

// WIFI Mode (station/softAP/station+softAP) (AT+CWMODE)
void esp8266_mode(unsigned char);

// Connect to AP (AT+CWJAP)
unsigned char esp8266_connect(unsigned char*, unsigned char*);

// Disconnect from AP (AT+CWQAP)
void esp8266_disconnect(void);

// Configuration of SoftAP Mode (AT+CWSAP)
void SAPmode(unsigned char* ssid, unsigned char* pass, unsigned int channel, unsigned int encryption);

// Multiple connection enable/disable (AT+CIPMUX)
void multipleConnect(unsigned char mode);

// Server Configuration mode
void configureServer(unsigned char mode, unsigned char *port);

// Local IP (AT+CIFSR)
void esp8266_ip(unsigned char*);

unsigned char esp8266_mac(unsigned char* store_in);

// Create connection (AT+CIPSTART)
unsigned char esp8266_start(unsigned char protocol, char* ip, unsigned char* port);

// Send data (AT+CIPSEND)
unsigned char esp8266_send(unsigned char* data, unsigned int length) ;

// Receive data (+IPD)
void esp8266_receive(unsigned char*, unsigned int, unsigned char);

/** Functions for internal use only **/

// Wait for a certain string on the input
 unsigned int _esp8266_waitFor(unsigned char *);
// Wait for any response on the input
 unsigned char _esp8266_waitResponse(void);

void clear(void);

#ifdef	__cplusplus
}
#endif

#endif	/* ESP8266_H */
