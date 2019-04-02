#ifndef HWIODEF_H
#define HWIODEF_H

#include "topfeature.h"

#define ESP8266_LED_PIN 2 //attached to ESP8266XX module inside NodeMCU. 
#define NODEMCU_LED_PIN 16 // attached to NodeMCU DevKit.
#define LED_ALIVE_PIN ESP8266_LED_PIN
#define LED_REPORT_PIN NODEMCU_LED_PIN
#define LED_CONNECT_PIN ESP8266_LED_PIN
#if defined(USE_LOG_TIMER) // Use LED_ALIVE_PIN.
    #undef LED_CONNECT_PIN
#endif

#if (0)
#define BUTTON_RST  16 // reset button.
#define BUTTON_USER BUTTON_RST
#endif

#define BUTTON_FLASH  0 // flash Button.
#define BUTTON_PIN  BUTTON_FLASH

enum {
  ERROR_UNKNOWN_COMMAND = -1,
  ERROR_I2C_NOT_CONFIGURED = -2,
  ERROR_I2C_READ_LIMIT_EXCEEDED = -3
};
#endif