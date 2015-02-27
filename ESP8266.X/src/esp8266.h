/* 
 * File:   esp8266.h
 * Author: Camil Staps <info@camilstaps.nl>
 */

#ifndef ESP8266_H
#define	ESP8266_H

#ifdef	__cplusplus
extern "C" {
#endif

#define ESP8266_STATION 0x01
#define ESP8266_SOFTAP 0x02

#define ESP8266_TCP 1
#define ESP8266_UDP 0

void esp8266_waitDone(void);        // Wait until the ESP has finished the last command
bit esp8266_isStarted(void);       // Check if the module is started (AT)
void esp8266_restart(void);         // Restart module (AT+RST)
void esp8266_echoCmds(bit);        // Enabled/disable command echoing (ATE)

// WIFI Mode (station/softAP/station+softAP) (AT+CWMODE)
void esp8266_mode(unsigned char);

// Connect to AP (AT+CWJAP)
bit esp8266_connect(unsigned char*, unsigned char*);

// Disconnect from AP (AT+CWQAP)
void esp8266_disconnect(void);

// Local IP (AT+CIFSR)
void esp8266_ip(char*);

// Create connection (AT+CIPSTART)
bit esp8266_start(bit protocol, char* ip, unsigned char port);

// Send data (AT+CIPSEND)
bit esp8266_send(unsigned char*);

// User implementation: in and output
void esp8266_putch(unsigned char);
unsigned char esp8266_getch(void);

#ifdef	__cplusplus
}
#endif

#endif	/* ESP8266_H */

