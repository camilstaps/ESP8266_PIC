/**
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

#include "esp8266.h"

/**
 * Check if the module is started
 *
 * This sends the `AT` command to the ESP and waits until it gets a response.
 *
 * @return true if the module is started, false if something went wrong
 */
bit esp8266_isStarted(void) {
    _esp8266_print("AT\r\n");
    return (_esp8266_waitResponse() == ESP8266_OK);
}

// Restart module (AT+RST)
/**
 * Restart the module
 *
 * This sends the `AT+RST` command to the ESP and waits until there is a
 * response.
 *
 * @return true iff the module restarted properly
 */
bit esp8266_restart(void) {
    _esp8266_print("AT+RST\r\n");
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
void esp8266_echoCmds(bool echo) {
    _esp8266_print("ATE");
    if (echo) {
        _esp8266_putch('1');
    } else {
        _esp8266_putch('0');
    }
    _esp8266_print("\r\n");
    _esp8266_waitOK();
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
    _esp8266_putch(mode + '0');
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
    _esp8266_waitOK();
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
    _esp8266_waitOK();
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
bit esp8266_start(bit protocol, char* ip, unsigned char port) {
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
bit esp8266_send(unsigned char* data) {
    return 1;
}

/**
 * Output a string to the ESP module.
 *
 * This is a function for internal use only.
 *
 * @param ptr A pointer to the string to send.
 */
void _esp8266_print(unsigned const char *ptr) {
    while (*ptr != 0) {
        _esp8266_putch(*ptr++);
    }
}

/**
 * Wait until we received 'OK' from the ESP.
 *
 * This is a function for internal use only.
 *
 * @see _esp8266_waitResponse for a more general function.
 */
inline void _esp8266_waitOK(void) {
    bool found_o = 0;
    bool found_ok = 0;
    unsigned char received;
    do {
        received = _esp8266_getch();
        if (found_o) {
            if (received == 'K') {
                found_ok = 1;
            } else {
                found_o = 0;
            }
        } else if (received == 'O') {
            found_o = 1;
        }
    } while (!found_ok);
}

/**
 * Wait until we received 'ready' from the ESP.
 *
 * This is a function for internal use only.
 *
 * @see _esp8266_waitResponse for a more general function.
 */
inline void _esp8266_waitReady(void) {
    unsigned const char* ready = "ready";
    unsigned char received;
    unsigned char received_so_far = 0;
    do {
        received = _esp8266_getch();
        if (received == ready[received_so_far]) {
            received_so_far++;
        } else {
            received_so_far = 0;
        }
    } while (received_so_far < 5);
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
 *
 * Not implemented yet:
 *  * DNS fail (or something like that)
 *
 * @return a constant from esp8266.h describing the status response.
 */
inline unsigned char _esp8266_waitResponse(void) {
    unsigned char so_far[4] = {0,0,0,0};
    unsigned const char lengths[4] = {2,5,4,9};
    unsigned const char* strings[4] = {"OK", "ready", "FAIL", "no change"};
    unsigned const char responses[4] = {ESP8266_OK, ESP8266_READY, ESP8266_FAIL, ESP8266_NOCHANGE};
    unsigned char received;
    unsigned char response;
    bool continue_loop = true;
    while (continue_loop) {
        received = _esp8266_getch();
        for (unsigned char i = 0; i < 4; i++) {
            if (strings[i][so_far[i]] == received) {
                so_far[i]++;
                if (so_far[i] == lengths[i]) {
                    response = responses[i];
                    continue_loop = false;
                }
            } else {
                so_far[i] = 0;
            }
        }
    }
    return response;
}