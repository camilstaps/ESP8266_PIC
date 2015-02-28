/* 
 * File:   esp8266.h
 * Author: Camil Staps <info@camilstaps.nl>
 */

#include <stdbool.h>

#ifndef ESP8266_H
#define	ESP8266_H

#ifdef	__cplusplus
extern "C" {
#endif

#define ESP8266_STATION 0x01
#define ESP8266_SOFTAP 0x02

#define ESP8266_TCP 1
#define ESP8266_UDP 0

#define ESP8266_OK 1
#define ESP8266_READY 2
#define ESP8266_FAIL 3
#define ESP8266_NOCHANGE 4

bit esp8266_isStarted(void);        // Check if the module is started (AT)
void esp8266_restart(void);         // Restart module (AT+RST)
void esp8266_echoCmds(bool);        // Enabled/disable command echoing (ATE)

// WIFI Mode (station/softAP/station+softAP) (AT+CWMODE)
void esp8266_mode(unsigned char);

// Connect to AP (AT+CWJAP)
unsigned char esp8266_connect(unsigned char*, unsigned char*);

// Disconnect from AP (AT+CWQAP)
void esp8266_disconnect(void);

// Local IP (AT+CIFSR)
void esp8266_ip(char*);

// Create connection (AT+CIPSTART)
bit esp8266_start(bit protocol, char* ip, unsigned char port);

// Send data (AT+CIPSEND)
bit esp8266_send(unsigned char*);

// User implementation: in and output
void _esp8266_putch(unsigned char);
unsigned char _esp8266_getch(void);

// Internal functions
// Print a string
void _esp8266_print(unsigned const char *);
// Wait until the ESP has sent 'OK'
inline void _esp8266_waitOK(void);
// Wait until the ESP has sent 'ready'
inline void _esp8266_waitReady(void);

inline unsigned char _esp8266_waitResponse(void);

#ifdef	__cplusplus
}
#endif

#endif	/* ESP8266_H */
