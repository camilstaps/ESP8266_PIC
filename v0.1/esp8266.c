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
 * File:    esp8266.c
 * Author:  Camil Staps <info@camilstaps.nl>
 * Website: http://github.com/camilstaps/ESP8266_PIC
 * Version: 0.1
 *
 * See:     esp8266.h
 *
 * C library for interfacing the ESP8266 Wi-Fi transceiver module with a PIC18
 * microcontroller. Should be used with the XC8 compiler.
 */
 
 /*The following modification was done by Markus Johansson and Nghi Nguyen */

#include "esp8266.h"
#include <stdio.h>
#include <string.h>
#include <delays.h>

unsigned char rx;
unsigned char rx_ok;
unsigned char test[11] = { 0 };
unsigned char test_j = 0;

unsigned int msCounter = 0;
unsigned char startmsCounter = 0;

void rx_handler(void);

#pragma code rx_interrupt = 0x8
void rx_int(void)
{
	_asm goto rx_handler _endasm
}
#pragma code

#pragma interrupt rx_handler
void rx_handler(void)
{
	if (PIR1bits.RCIF == 1) // check if uart filled
	{
		rx = RCREG; // read new data into variable
		rx_ok = 1; // new data received. so enable flg
		PIR1bits.RCIF = 0;
	}

	// Timer2 Interrupt- Freq = 1000.00 Hz - Period = 0.001000 seconds
	if (PIR1bits.TMR2IF == 1)          // timer 2 interrupt flag
	{
		if (startmsCounter == 1)
			msCounter++;
		PIR1bits.TMR2IF = 0;     // clears TMR2IF       bit 1 TMR2IF: TMR2 to PR2 Match Interrupt Flag bit
	}
}

/**
* Output a string to the ESP module.
*
* This is a function for internal use only.
*
* @param ptr A pointer to the string to send.
*/
void _esp8266_print(unsigned const char *ptr)
{
	while (BusyUSART());
	printf(ptr);
}

void _esp8266_putch(unsigned char c)
{
	while (BusyUSART());
	WriteUSART(c);
}

unsigned char _esp8266_getch(void)
{
	return rx;
}
/**
 * Check if the module is started
 *
 * This sends the `AT` command to the ESP and waits until it gets a response.
 *
 * @return true if the module is started, false if something went wrong
 */
unsigned char esp8266_isStarted(void) {
	_esp8266_print("AT\r\n");
	return (_esp8266_waitResponse() == ESP8266_OK);
}

/**
 * Restart the module
 *
 * This sends the `AT+RST` command to the ESP and waits until there is a
 * response.
 *
 * @return true iff the module restarted properly
 */
unsigned char esp8266_restart(void) {
	_esp8266_print("AT+RST\r\n");
//	if (_esp8266_waitResponse() != ESP8266_OK) {
//	return 0;
//	}
	return (_esp8266_waitResponse() == ESP8266_READY);
}

/**
 * Enable / disable command echoing.
 *
 * Enabling this is useful for debugging: one could sniff the TX line from the
 * ESP8266 with his computer and thus receive both commands and responses.
 *
 * This sends the ATE command to the ESP module.
 *
 * @param echo whether to enable command echoing or not
 */
void esp8266_echoCmds(unsigned char echo) {
	_esp8266_print("ATE");
	if (echo) {
		_esp8266_putch('1');
	}
	else {
		_esp8266_putch('0');
	}
	_esp8266_print("\r\n");
	_esp8266_waitFor("OK");
}

/**
 * Set the WiFi mode.
 *
 * ESP8266_STATION : Station mode
 * ESP8266_SOFTAP : Access point mode
 *
 * This sends the AT+CWMODE command to the ESP module.
 *
 * @param mode an ORed bitmask of ESP8266_STATION and ESP8266_SOFTAP
 */
void esp8266_mode(unsigned char mode) {
	_esp8266_print("AT+CWMODE=");
	//_esp8266_putch(mode);
	printf("%u", mode);
	_esp8266_print("\r\n");
	_esp8266_waitResponse();
}

/**
 * Connect to an access point.
 *
 * This sends the AT+CWJAP command to the ESP module.
 *
 * @param ssid The SSID to connect to
 * @param pass The password of the network
 * @return an ESP status code, normally either ESP8266_OK or ESP8266_FAIL
 */
unsigned char esp8266_connect(unsigned char* ssid, unsigned char* pass) {
    _esp8266_print("AT+CWJAP=\"");
    _esp8266_print(ssid);
    _esp8266_print("\",\"");
    _esp8266_print(pass);
    _esp8266_print("\"\r\n");
    return _esp8266_waitResponse();
}

/**
 * Disconnect from the access point.
 *
 * This sends the AT+CWQAP command to the ESP module.
 */
void esp8266_disconnect(void) {
	_esp8266_print("AT+CWQAP\r\n");
	//_esp8266_waitFor("OK");
	_esp8266_waitResponse();
}

/* Configuration of softAP mode */
void SAPmode(unsigned char* ssid, unsigned char* pass, unsigned int channel, unsigned int encryption) {
	_esp8266_print("AT+CWSAP=\"");
	_esp8266_print(ssid);
	_esp8266_print("\",\"");
	_esp8266_print(pass);
    _esp8266_print("\",");
    _esp8266_print(channel);
    _esp8266_print(",");
    _esp8266_print(encryption);
	_esp8266_print("\r\n");
	
    //return _esp8266_waitResponse();
    //_esp8266_waitFor("OK");
    _esp8266_waitResponse();
}

/* Multiple connections enable/ disable 
   mode: 0 = single connection
         1 = multiple connections (max 4)
   Note: This mode can only be CHANGED if all connections are DISCONNECTED */
void multipleConnect(unsigned char mode)
{
    _esp8266_print("AT+CIPMUX=");
    printf("%u", mode);
    _esp8266_print("\r\n");
    _esp8266_waitResponse();
}

/* Configure as Server mode
   mode: 0 = delete server (must be followed by restart)
         1 = create server
   port: port number, default is 333*/
void configureServer(unsigned char mode, unsigned char* port)
{
    _esp8266_print("AT+CIPSERVER=");
    printf("%u", mode);
    _esp8266_print(",");
    _esp8266_print(port);
    _esp8266_print("\r\n");
    
    _esp8266_waitResponse();
}

/**
 * Store the current local IPv4 address.
 *
 * This sends the AT+CIFSR command to the ESP module.
 *
 * The result will not be stored as a string but byte by byte. For example, for
 * the IP 192.168.0.1, the value of store_in will be: {0xc0, 0xa8, 0x00, 0x01}.
 *
 * @param store_in a pointer to an array of the type unsigned char[4]; this
 * array will be filled with the local IP.
 */
void esp8266_ip(unsigned char* store_in) {
    _esp8266_print("AT+CIFSR\r\n");
    unsigned char received;
    do {
        received = _esp8266_getch();
    } while (received < '0' || received > '9');
    for (unsigned char i = 0; i < 4; i++) {
        store_in[i] = 0;
        do {
            store_in[i] = 10 * store_in[i] + received - '0';
            received = _esp8266_getch();
        } while (received >= '0' && received <= '9');
        received = _esp8266_getch();
    }
    _esp8266_waitFor("OK");
}

/* Function get IP address */
void getIP(unsigned char* store_in)
{
	unsigned char prev[4] = { 0 };
	unsigned char received;
	unsigned char i = 0;

	_esp8266_print("AT+CIFSR\r\n");

	msCounter = 0;
	startmsCounter = 1;
	do
	{
		if (msCounter >= 6000)
		{
			startmsCounter = 0;
			return ESP8266_TIMEOUT;
		}

		if (rx_ok == 1)
		{
			rx_ok = 0;
			received = _esp8266_getch();
			if (received == 'A')
				prev[0] = received;
			if (received == 'P')
				prev[1] = received;
			if (received == 'I')
				prev[2] = received;
			if (received == 'P')
				prev[3] = received;
		}
	} while (!((prev[0] == 'A') && (prev[1] == 'P') && (prev[2] == 'I') && (prev[3] == 'P')));
	// +CIFSR:APIP,"IP is filled here"<\r><\n>

	msCounter = 0;
	startmsCounter = 1;
	rx_ok = 0;
	i = 0;
	do
	{
		if (msCounter >= 6000)
		{
			startmsCounter = 0;
			return ESP8266_TIMEOUT;
		}

		if (rx_ok == 1)
		{
			rx_ok = 0;
			received = _esp8266_getch();
			if ((received != 'A') && (received != 'C"') && (received != ',') && (received != '\"') && (received != '\r'))
				store_in[i++] = received;
		}
	} while (!(received == '\r') || (received == '\n'));
    
    store_in[i] = '\0';

	clear();
	_esp8266_waitFor("OK");
}

/* Function to get the MAC address */
unsigned char esp8266_mac(unsigned char* store_in)
{
	unsigned char prev[6] = { 0 };
	unsigned char received;
	unsigned char i = 0;

	_esp8266_print("AT+CIFSR\r\n");

	msCounter = 0;
	startmsCounter = 1;
	do
	{
		if (msCounter >= 6000)
		{
			startmsCounter = 0;
			return ESP8266_TIMEOUT;
		}

		if (rx_ok == 1)
		{
			rx_ok = 0;
			received = _esp8266_getch();
			if (received == 'S')
				prev[0] = received;
			if (received == 'T')
				prev[1] = received;
			if (received == 'A')
				prev[2] = received;
			if (received == 'M')
				prev[3] = received;
			if (received == 'A')
				prev[4] = received;
			if (received == 'C')
				prev[5] = received;
		}
	} while (!((prev[0] == 'S') && (prev[1] == 'T') && (prev[2] == 'A') && (prev[3] == 'M') && (prev[4] == 'A') && (prev[5] == 'C')));
	// +CIFSR:STAMAC,"18:fe:34:a5:75:2f"<\r><\n>

	msCounter = 0;
	startmsCounter = 1;
	rx_ok = 0;
	i = 0;
	do
	{
		if (msCounter >= 6000)
		{
			startmsCounter = 0;
			return ESP8266_TIMEOUT;
		}

		if (rx_ok == 1)
		{
			rx_ok = 0;
			received = _esp8266_getch();
			if ((received != 'A') && (received != 'C"') && (received != ',') && (received != '\"') && (received != '\r'))
				store_in[i++] = received;
		}
	} while (!(received == '\r') || (received == '\n'));
	store_in[17] = '\0';

	clear();
	_esp8266_waitFor("OK");
	return ESP8266_OK;
}

/**
 * Open a TCP or UDP connection.
 *
 * This sends the AT+CIPSTART command to the ESP module.
 *
 * @param protocol Either ESP8266_TCP or ESP8266_UDP
 * @param ip The IP or hostname to connect to; as a string
 * @param port The port to connect to
 *
 * @return true iff the connection is opened after this.
 */
unsigned char esp8266_start(unsigned char protocol, char* ip, unsigned char* port)
{
	_esp8266_print("AT+CIPSTART=\"");
	if (protocol == ESP8266_TCP) {
		_esp8266_print("TCP");
	}
	else {
		_esp8266_print("UDP");
	}
	_esp8266_print("\",\"");
	_esp8266_print(ip);
	_esp8266_print("\",");
	_esp8266_print(port);
	_esp8266_print("\r\n");
	/*if (_esp8266_waitResponse() != ESP8266_OK) {
	return 0;
	}*/
	if (_esp8266_waitResponse() != ESP8266_LINKED) {
		return 0;
	}
	return 1;
}

// Send data (AT+CIPSEND)
/**
 * Send data over a connection.
 *
 * This sends the AT+CIPSEND command to the ESP module.
 *
 * @param data The data to send
 *
 * @return true iff the data was sent correctly.
 */
unsigned char esp8266_send(unsigned char* data, unsigned int length)
{
	unsigned int i = 0;
	_esp8266_print("AT+CIPSEND=");

	while (BusyUSART());
	printf("%u", length);

	_esp8266_print("\r\n");
	msCounter = 0;
	startmsCounter = 1;
	do
	{
		if (msCounter >= 6000)
		{
			startmsCounter = 0;
			return ESP8266_TIMEOUT;
		}
	} while (_esp8266_getch() != '>');
	Delay10KTCYx(20);
	for (i = 0; i<length; i++)
		_esp8266_putch(data[i]);

	//_esp8266_print(data);
	//_esp8266_print("\r\n");
	if (_esp8266_waitResponse() == ESP8266_SEND_OK) {
		return 1;
	}
	return 0;
}

/**
 * Read a string of data that is sent to the ESP8266.
 *
 * This waits for a +IPD line from the module. If more bytes than the maximum
 * are received, the remaining bytes will be discarded.
 *
 * @param store_in a pointer to a character array to store the data in
 * @param max_length maximum amount of bytes to read in
 * @param discard_headers if set to true, we will skip until the first \r\n\r\n,
 * for HTTP this means skipping the headers.
 */
void esp8266_receive(unsigned char* store_in, unsigned int max_length, unsigned char discard_headers) {
	unsigned int length = 0;
	unsigned int i;
	unsigned char received = _esp8266_getch();
	_esp8266_waitFor("+IPD,");
	do {
		length = length * 10 + received - '0';
		received = _esp8266_getch();
	} while (received >= '0' && received <= '9');

	if (discard_headers) {
		length -= _esp8266_waitFor("\r\n\r\n");
	}

	if (length < max_length) {
		max_length = length;
	}

	/*sprintf(store_in, "%u,%u:%c%c", length, max_length, _esp8266_getch(), _esp8266_getch());
	return;*/

	for (i = 0; i < max_length; i++) {
		store_in[i] = _esp8266_getch();
	}
	store_in[i] = 0;
	for (; i < length; i++) {
		_esp8266_getch();
	}
	_esp8266_waitFor("OK");
}
/**
 * Output a string to the ESP module.
 *
 * This is a function for internal use only.
 *
 * @param ptr A pointer to the string to send.
 */
/*void _esp8266_print(unsigned const char *ptr) {
    while (*ptr != 0) {
        _esp8266_putch(*ptr++);
    }
}*/

/**
 * Wait until we found a string on the input.
 *
 * Careful: this will read everything until that string (even if it's never
 * found). You may lose important data.
 *
 * @param string
 *
 * @return the number of characters read
 */
unsigned int _esp8266_waitFor(unsigned char *string) {
	unsigned char so_far = 0;
	unsigned char received;
	unsigned int counter = 0;
	unsigned int i = 0;

	msCounter = 0;
	startmsCounter = 1;
	do
	{
		if (msCounter >= 6000)
		{
			startmsCounter = 0;
			return ESP8266_TIMEOUT;
		}

		if (rx_ok == 1)
		{
			rx_ok = 0;
			received = _esp8266_getch();
			test[test_j++] = received;

			if (test_j >= 10 || received == '\n')
				test_j = 0;

			counter++;
		}

		if (test_j == 0)
		{
			for (i = 0; i<counter; i++)
			{
				if (test[i] == string[so_far])
					so_far++;
				else
					so_far = 0;
			}
		}
	} while (string[so_far] != 0);
	return counter;
}

/**
 * Wait until we received the ESP is done and sends its response.
 *
 * This is a function for internal use only.
 *
 * Currently the following responses are implemented:
 *  * OK
 *  * ready
 *  * FAIL
 *  * no change
 *  * Linked
 *  * Unlink
 *
 * Not implemented yet:
 *  * DNS fail (or something like that)
 *
 * @return a constant from esp8266.h describing the status response.
 */
unsigned char _esp8266_waitResponse(void) {
	unsigned char so_far[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	unsigned const char lengths[11] = { 2, 5, 4, 6, 6, 6, 5, 5, 6, 6, 6 };//{2,5,4,9,7,8,5,5,9,7};
	unsigned const char strings[11][6] = {
		{ 'O', 'K', '0', '0', '0', '0' },
		{ 'r', 'e', 'a', 'd', 'y', '0' },
		{ 'F', 'A', 'I', 'L', '0', '0' },
		{ 'n', 'o', ' ', 'c', 'h', 'a' },
		{ 'C', 'O', 'N', 'N', 'E', 'C' },
		{ '0', ',', 'C', 'L', 'O', 'S' },
		{ 'E', 'R', 'R', 'O', 'R', '0' },
		{ 'n', 'o', ' ', 'i', 'p', '0' },
		{ 't', 'y', 'p', 'e', ' ', 'e' },
		{ 'S', 'E', 'N', 'D', ' ', 'O' },
		{ 'l', 'i', 'n', 'k', ' ', 'i' } // link is not
		/*
		{'O','K','0','0','0','0','0','0','0'},
		{'r','e','a','d','y','0','0','0','0'},
		{'F','A','I','L','0','0','0','0','0'},
		{'n','o',' ','c','h','a','n','g','e'},
		{'C','O','N','N','E','C','T','0','0'},
		{'0',',','C','L','O','S','E','D','0'},
		{'E','R','R','O','R','0','0','0','0'},
		{'n','o',' ','i','p','0','0','0','0'},
		{'t','y','p','e',' ','e','r','r','o'},
		{'S','E','N','D',' ','O','K','0','0'}*/
	};
	unsigned const char responses[11] = { ESP8266_OK, ESP8266_READY, ESP8266_FAIL, ESP8266_NOCHANGE, ESP8266_LINKED, ESP8266_UNLINK, ESP8266_ERROR, ESP8266_NO_IP, ESP8266_TYPE_ERROR, ESP8266_SEND_OK, ESP8266_NO_LINK };
	unsigned char received;
	unsigned char response;
	unsigned char continue_loop = 1;
	unsigned char i = 0;
	unsigned char j = 0;

	unsigned char check = 0;

	test_j = 0;

	/*for (i=0; i<6; i++)
	{
	for(j=0; j<lengths[i]; j++)
	_esp8266_putch(strings[i][j]);
	} */
	msCounter = 0;
	startmsCounter = 1;
	while (continue_loop)
	{
		if (msCounter >= 6000)
		{
			startmsCounter = 0;
			return ESP8266_TIMEOUT;
		}

		if (rx_ok == 1)
		{
			rx_ok = 0;
			received = _esp8266_getch();

			test[test_j++] = received;
			if (test_j >= 10 || received == '\n')
				test_j = 0;
		}

		if (test_j == 0)
		{
			for (i = 0; i<11; i++)
			{
				for (j = 0; j<lengths[i]; j++)
				{
					if (strings[i][j] == test[j])
					{
						so_far[i]++;
						if (so_far[i] == lengths[i])
						{
							response = responses[i];
							continue_loop = 0;
							break;
						}
					}
					else
						so_far[i] = 0;
				}
			}
		}
	}
	return response;
}

/*Function to clear command after executed*/
void clear(void)
{
	unsigned char i = 0;

	for (i = 0; i<11; i++)
		test[i] = 0;
}
