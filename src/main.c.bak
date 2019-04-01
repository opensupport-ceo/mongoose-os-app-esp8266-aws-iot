/*
 * Copyright (c) 2014-2018 OWL Software Limited
 * All rights reserved
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

#include "mgos.h"
#include "mgos_mqtt.h"
#include "mgos_dht.h"

#define USE_DHT
//#undef USE_DHT

#define ESP8266_LED_PIN 2 //attached to ESP8266XX module inside NodeMCU. 
#define NODEMCU_LED_PIN 16 // attached to NodeMCU DevKit.
#define LED_ALIVE_PIN ESP8266_LED_PIN
#define LED_REPORT_PIN NODEMCU_LED_PIN
#define LED_CONNECT_PIN LED_ALIVE_PIN

#if (0)
#define BUTTON_RST  16 // reset button.
#define BUTTON_USER BUTTON_RST
#endif

#define BUTTON_FLASH  0 // flash Button.
#define BUTTON_PIN  BUTTON_FLASH 

#if defined(USE_DHT)
static struct mgos_dht *s_dht = NULL;
#endif

static void led_timer_cb(void *arg) {
  /*bool val = mgos_gpio_toggle(2);*/
  static bool s_tick_tock = false;
  LOG(LL_INFO,
      ("%s uptime: %.2lf, RAM: %lu, %lu free", (s_tick_tock ? "Tick" : "Tock"),
       mgos_uptime(), (unsigned long) mgos_get_heap_size(),
       (unsigned long) mgos_get_free_heap_size()));
  s_tick_tock = !s_tick_tock;
  mgos_gpio_toggle(LED_ALIVE_PIN);
  (void) arg;
}

#ifdef LED_CONNECT_PIN
static void con_timer_cb(void *arg) {
  /*bool val = mgos_gpio_toggle(2);*/
  static bool s_tick_tock = false;
#if defined(USE_DHT)
  LOG(LL_INFO,
      ("%s uptime: %.2lf, RAM: %lu, %lu free, temperature: %f, humidity: %f", (s_tick_tock ? "Tick" : "Tock"),
       mgos_uptime(), (unsigned long) mgos_get_heap_size(),
       (unsigned long) mgos_get_free_heap_size(),
       (float) mgos_dht_get_temp(s_dht),
       (float) mgos_dht_get_humidity(s_dht)));
#else  
  LOG(LL_INFO,
      ("%s uptime: %.2lf, RAM: %lu, %lu free", (s_tick_tock ? "Tick" : "Tock"),
       mgos_uptime(), (unsigned long) mgos_get_heap_size(),
       (unsigned long) mgos_get_free_heap_size()));
#endif
  s_tick_tock = !s_tick_tock;
  mgos_gpio_toggle(LED_CONNECT_PIN);
  (void) arg;
}
#endif

static void net_cb(int ev, void *evd, void *arg) {
  switch (ev) {
    case MGOS_NET_EV_DISCONNECTED:
      LOG(LL_INFO, ("%s", "Net disconnected"));
      break;
    case MGOS_NET_EV_CONNECTING:
      LOG(LL_INFO, ("%s", "Net connecting..."));
      break;
    case MGOS_NET_EV_CONNECTED:
      LOG(LL_INFO, ("%s", "Net connected"));
  #ifdef LED_CONNECT_PIN
      mgos_set_timer(1000 /* ms */, MGOS_TIMER_REPEAT, con_timer_cb, NULL);
  #endif    
      break;
    case MGOS_NET_EV_IP_ACQUIRED:
      LOG(LL_INFO, ("%s", "Net got IP address"));
      break;
  }
 
  (void) evd;
  (void) arg;
}
 
static void report_temperature(void *arg) {
  char topic[100], message[160];
  struct json_out out = JSON_OUT_BUF(message, sizeof(message));
   
  time_t now=time(0);
  struct tm *timeinfo = localtime(&now);
 
  snprintf(topic, sizeof(topic), "esp8266/event/temp_humidity");
#if defined(USE_DHT)
  json_printf(&out, "{total_ram: %lu, free_ram: %lu, temperature: %f, humidity: %f, device: \"%s\", timestamp: \"%02d:%02d:%02d\"}",
              (unsigned long) mgos_get_heap_size(),
              (unsigned long) mgos_get_free_heap_size(),
              (float) mgos_dht_get_temp(s_dht), 
              (float) mgos_dht_get_humidity(s_dht),
              (char *) mgos_sys_config_get_device_id(),
              (int) timeinfo->tm_hour,
              (int) timeinfo->tm_min,
              (int) timeinfo->tm_sec);
#else
    json_printf(&out, "{total_ram: %lu, free_ram: %lu, device: \"%s\", timestamp: \"%02d:%02d:%02d\"}",
              (unsigned long) mgos_get_heap_size(),
              (unsigned long) mgos_get_free_heap_size(),
              (char *) mgos_sys_config_get_device_id(),
              (int) timeinfo->tm_hour,
              (int) timeinfo->tm_min,
              (int) timeinfo->tm_sec);
#endif
  bool res = mgos_mqtt_pub(topic, message, strlen(message), 1, false);
  LOG(LL_INFO, ("Published to MQTT: %s", res ? "yes" : "no"));
  mgos_gpio_toggle(LED_REPORT_PIN);
  (void) arg;
}
 
static void button_cb(int pin, void *arg) {
#if defined(USE_DHT)
  float t = mgos_dht_get_temp(s_dht);
  float h = mgos_dht_get_humidity(s_dht);
#endif
  LOG(LL_INFO, ("Button presses on pin: %d", pin));
#if defined(USE_DHT) 
  LOG(LL_INFO, ("Temperature: %f *C Humidity: %f %%\n", t, h));
#endif

  report_temperature(NULL);
  (void) arg;
}

enum mgos_app_init_result mgos_app_init(void) {

#ifndef LED_CONECT_PIN
  /* Blink built-in NodeMCU LED every second */
  mgos_gpio_set_mode(LED_ALIVE_PIN, MGOS_GPIO_MODE_OUTPUT);
  //mgos_gpio_setup_output(LED_ALIVE_PIN, 0);
  
  mgos_set_timer(1000 /* ms */, MGOS_TIMER_REPEAT, led_timer_cb, NULL);
#else
  /* Blink built-in NodeMCU LED every second */
  mgos_gpio_set_mode(LED_CONNECT_PIN, MGOS_GPIO_MODE_OUTPUT);
#endif
  /* Blink built-in ESP8266XX LED every second */
  mgos_gpio_set_mode(LED_REPORT_PIN, MGOS_GPIO_MODE_OUTPUT);

  /* Report temperature to AWS IoT Core every 5 mins */
  mgos_set_timer(300000, MGOS_TIMER_REPEAT, report_temperature, NULL);

  /* Publish to MQTT on button press */
  mgos_gpio_set_button_handler(0,
                               MGOS_GPIO_PULL_UP, MGOS_GPIO_INT_EDGE_NEG, 200,
                               button_cb, NULL);

#if defined(USE_DHT)
  if ((s_dht = mgos_dht_create(5, DHT11)) == NULL) {
    LOG(LL_INFO, ("Unable to initialize DHT11"));
  }
#endif

  /* Network connectivity events */
  mgos_event_add_group_handler(MGOS_EVENT_GRP_NET, net_cb, NULL);

  return MGOS_APP_INIT_SUCCESS;
}

