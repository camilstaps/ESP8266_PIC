#include "esp8266.h"

// Wait until the ESP has finished the last command
void esp8266_waitDone(void) {

}

// Check if the module is started (AT)
bit esp8266_isStarted(void) {
    return 1;
}

// Restart module (AT+RST)
void esp8266_restart(void) {

}

// Enabled/disable command echoing (ATE)
void esp8266_echoCmds(bit echo) {

}

// WIFI Mode (station/softAP/station+softAP) (AT+CWMODE)
void esp8266_mode(unsigned char) {

}

// Connect to AP (AT+CWJAP)
bit esp8266_connect(unsigned char*, unsigned char*) {
    return 1;
}

// Disconnect from AP (AT+CWQAP)
void esp8266_disconnect(void) {

}

// Local IP (AT+CIFSR)
void esp8266_ip(char*) {

}

// Create connection (AT+CIPSTART)
bit esp8266_start(bit protocol, char* ip, unsigned char port) {
    return 1;
}

// Send data (AT+CIPSEND)
bit esp8266_send(unsigned char*) {
    return 1;
}