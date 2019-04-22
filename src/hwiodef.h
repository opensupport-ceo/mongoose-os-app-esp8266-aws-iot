/*
 * Copyright (c) 2014-2019 OWL Software Limited
 * All rights reserved
 * 
 * Author: Jaehong Park, jaehong1972@gmail.com
 * 
 * Description: Board-specific definition header file.
 *
 * Licensed under the Apache License, Version 2.0 (the ""License"");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ""AS IS"" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
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