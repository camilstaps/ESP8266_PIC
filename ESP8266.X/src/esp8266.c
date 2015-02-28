#include "esp8266.h"

// Check if the module is started (AT)
bit esp8266_isStarted(void) {
    _esp8266_print("AT\r\n");
    _esp8266_waitOK();
    return 1;
}

// Restart module (AT+RST)
void esp8266_restart(void) {
    _esp8266_print("AT+RST\r\n");
    _esp8266_waitReady();
}

// Enabled/disable command echoing (ATE)
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

// WIFI Mode (station/softAP/station+softAP) (AT+CWMODE)
void esp8266_mode(unsigned char mode) {
    _esp8266_print("AT+CWMODE=");
    _esp8266_putch(mode + '0');
    _esp8266_print("\r\n");
    _esp8266_waitResponse();
}

// Connect to AP (AT+CWJAP)
unsigned char esp8266_connect(unsigned char* ssid, unsigned char* pass) {
    _esp8266_print("AT+CWJAP=\"");
    _esp8266_print(ssid);
    _esp8266_print("\",\"");
    _esp8266_print(pass);
    _esp8266_print("\"\r\n");
    return _esp8266_waitResponse();
}

// Disconnect from AP (AT+CWQAP)
void esp8266_disconnect(void) {
    _esp8266_print("AT+CWQAP\r\n");
    _esp8266_waitOK();
}

// Local IP (AT+CIFSR)
void esp8266_ip(char* store_in) {

}

// Create connection (AT+CIPSTART)
bit esp8266_start(bit protocol, char* ip, unsigned char port) {
    return 1;
}

// Send data (AT+CIPSEND)
bit esp8266_send(unsigned char*) {
    return 1;
}

// Print a string
void _esp8266_print(unsigned const char *ptr) {
    while (*ptr != 0) {
        _esp8266_putch(*ptr++);
    }
}

// Wait until the ESP has sent 'OK'
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

// Wait until the ESP has sent 'ready'
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