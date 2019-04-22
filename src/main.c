/*
 * Copyright (c) 2014-2019 OWL Software Limited
 * All rights reserved
 * 
 * Author: Jaehong Park, jaehong1972@gmail.com
 *
 * Description: Sample connected to AWS IoT Cloud via Mqtt. 
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

#include "topfeature.h"
#include "mgos.h"
#include "mgos_mqtt.h"
#include "mgos_dht.h"
#include "hwiodef.h"
 
#ifndef USE_JS_AWS_TEST
#if defined(USE_DHT)
static struct mgos_dht *s_dht = NULL;
#endif

#if defined(USE_LOG_TIMER)
static void log_timer_cb(void *arg) {
  /*bool val = mgos_gpio_toggle(2);*/
  static bool s_tick_tock = false;
  LOG(LL_INFO,
      ("%s uptime: %.2lf, RAM: %lu, %lu free", (s_tick_tock ? "Tick" : "Tock"),
       mgos_uptime(), (unsigned long) mgos_get_heap_size(),
       (unsigned long) mgos_get_free_heap_size()));
  s_tick_tock = !s_tick_tock;
  //mgos_gpio_toggle(LED_ALIVE_PIN);
  (void) arg;
}
#elif defined(LED_ALIVE_PIN)
static void log_timer_cb(void *arg) {
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
#endif

#if defined(USE_NET_CB) && defined(LED_CONNECT_PIN)
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

#if defined(USE_NET_CB)
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
#endif

#ifdef USE_PORT_MQTT
static void sub(struct mg_connection *c, const char *fmt, ...) {
  char buf[100];
  struct mg_mqtt_topic_expression te = {.topic = buf, .qos = 1};
  uint16_t sub_id = mgos_mqtt_get_packet_id();
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  mg_mqtt_subscribe(c, &te, 1, sub_id);
  LOG(LL_INFO, ("Subscribing to %s (id %u)", buf, sub_id));
}

static void pub(struct mg_connection *c, const char *fmt, ...) {
  char msg[200];
  struct json_out jmo = JSON_OUT_BUF(msg, sizeof(msg));
  va_list ap;
  int n;
  va_start(ap, fmt);
  n = json_vprintf(&jmo, fmt, ap);
  va_end(ap);
  mg_mqtt_publish(c, mgos_sys_config_get_mqtt_pub(), mgos_mqtt_get_packet_id(),
                  MG_MQTT_QOS(1), msg, n);
  LOG(LL_INFO, ("%s -> %s", mgos_sys_config_get_mqtt_pub(), msg));
}

#if (0) //Currently, not used here.
static uint8_t from_hex(const char *s) {
#define HEXTOI(x) (x >= '0' && x <= '9' ? x - '0' : x - 'W')
  int a = tolower(*(const unsigned char *) s);
  int b = tolower(*(const unsigned char *) (s + 1));
  return (HEXTOI(a) << 4) | HEXTOI(b);
}
#endif

static void gpio_int_handler(int pin, void *arg) {
  static double last = 0;
  double now = mg_time();
  if (now - last > 0.2) {
    struct mg_connection *c = mgos_mqtt_get_global_conn();
    last = now;
    if (c != NULL) {
      pub(c, "{type: %Q, pin: %d}", "click", pin);
    }
    LOG(LL_INFO, ("Click!"));
  }
  (void) arg;
}

static void ev_handler(struct mg_connection *c, int ev, void *p,
                       void *user_data) {
  struct mg_mqtt_message *msg = (struct mg_mqtt_message *) p;

  if (ev == MG_EV_MQTT_CONNACK) {
    LOG(LL_INFO, ("CONNACK: %d", msg->connack_ret_code));
    if (mgos_sys_config_get_mqtt_sub() == NULL ||
        mgos_sys_config_get_mqtt_pub() == NULL) {
      LOG(LL_ERROR, ("Run 'mgos config-set mqtt.sub=... mqtt.pub=...'"));
    } else {
      sub(c, "%s", mgos_sys_config_get_mqtt_sub());
    }
  } else if (ev == MG_EV_MQTT_SUBACK) {
    LOG(LL_INFO, ("Subscription %u acknowledged", msg->message_id));
  } else if (ev == MG_EV_MQTT_PUBLISH) {
    struct mg_str *s = &msg->payload;
  #if defined(USE_I2C)
    struct json_token t = JSON_INVALID_TOKEN;
    char buf[100], asciibuf[sizeof(buf) * 2 + 1];
    int pin, state, i, addr, len;
  #else
    int pin, state;
  #endif

    LOG(LL_INFO, ("got command: [%.*s]", (int) s->len, s->p));
    /* Our subscription is at QoS 1, we must acknowledge messages sent to us. */
    mg_mqtt_puback(c, msg->message_id);
    if (json_scanf(s->p, s->len, "{gpio: {pin: %d, state: %d}}", &pin,
                   &state) == 2) {
      /* Set GPIO pin to a given state */
      mgos_gpio_set_mode(pin, MGOS_GPIO_MODE_OUTPUT);
      mgos_gpio_write(pin, (state > 0 ? 1 : 0));
      pub(c, "{type: %Q, pin: %d, state: %d}", "gpio", pin, state);
    } else if (json_scanf(s->p, s->len, "{button: {pin: %d}}", &pin) == 1) {
      /* Report button press on GPIO pin to a publish topic */
      mgos_gpio_set_button_handler(pin, MGOS_GPIO_PULL_UP,
                                   MGOS_GPIO_INT_EDGE_POS, 50, gpio_int_handler,
                                   NULL);
      pub(c, "{type: %Q, pin: %d}", "button", pin);
    } 
  #if defined(USE_I2C) //Later, shall be verified.
    else if (json_scanf(s->p, s->len, "{i2c_read: {addr: %d, len: %d}}",
                          &addr, &len) == 2) {
      /* Read from I2C */
      struct mgos_i2c *i2c = mgos_i2c_get_global();
      if (len <= 0 || len > (int) sizeof(buf)) {
        pub(c, "{error: {code: %d, message: %Q}}",
            ERROR_I2C_READ_LIMIT_EXCEEDED, "Too long read");
      } else if (i2c == NULL) {
        pub(c, "{error: {code: %d, message: %Q}}", ERROR_I2C_NOT_CONFIGURED,
            "I2C is not enabled");
      } else {
        bool ret;
        asciibuf[0] = '\0';
        ret = mgos_i2c_read(i2c, addr, (uint8_t *) buf, len, true /* stop */);
        if (ret) {
          for (i = 0; i < len; i++) {
            const char *hex = "0123456789abcdef";
            asciibuf[i * 2] = hex[(((uint8_t *) buf)[i] >> 4) & 0xf];
            asciibuf[i * 2 + 1] = hex[((uint8_t *) buf)[i] & 0xf];
          }
          asciibuf[i * 2] = '\0';
        }
        pub(c, "{type: %Q, status: %d, data: %Q}", "i2c_read", ret, asciibuf);
      }
    } 
    else if (json_scanf(s->p, s->len, "{i2c_write: {data: %T}}", &t) == 1) {
      /* Write byte sequence to I2C. First byte is the address */
      struct mgos_i2c *i2c = mgos_i2c_get_global();
      if (i2c == NULL) {
        pub(c, "{error: {code: %d, message: %Q}}", ERROR_I2C_NOT_CONFIGURED,
            "I2C is not enabled");
      } else {
        bool ret;
        int j = 0;
        for (int i = 0; i < t.len; i += 2, j++) {
          ((uint8_t *) t.ptr)[j] = from_hex(t.ptr + i);
        }
        ret = mgos_i2c_write(i2c, t.ptr[0], t.ptr + 1, j, true /* stop */);
        pub(c, "{type: %Q, status: %d}", "i2c_write", ret);
      }
    } 
  #endif//#if defined(USE_I2C)
    else {
      pub(c, "{error: {code: %d, message: %Q}}", ERROR_UNKNOWN_COMMAND,
          "unknown command");
    }
  }
  (void) user_data;
}
#endif //USE_PORT_MQTT

#ifdef USE_REPORT_TEMP
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
#endif

static void button_cb(int pin, void *arg) {
#if defined(USE_DHT)
  float t = mgos_dht_get_temp(s_dht);
  float h = mgos_dht_get_humidity(s_dht);
#endif
  LOG(LL_INFO, ("Button presses on pin: %d", pin));
#if defined(USE_DHT) 
  LOG(LL_INFO, ("Temperature: %f *C Humidity: %f %%\n", t, h));
#endif

#ifdef USE_REPORT_TEMP
  report_temperature(NULL);
#endif

  (void) arg;
}
#endif//#ifndef USE_JS_AWS_TEST

#ifdef USE_JS_AWS_TEST
enum mgos_app_init_result mgos_app_init(void) {
  return MGOS_APP_INIT_SUCCESS;
}
#else
enum mgos_app_init_result mgos_app_init(void) {

#if defined(LED_CONNECT_PIN)
  /* Blink built-in NodeMCU LED every second */
  mgos_gpio_set_mode(LED_CONNECT_PIN, MGOS_GPIO_MODE_OUTPUT);
  //mgos_set_timer(1000 /* ms */, MGOS_TIMER_REPEAT, led_timer_cb, NULL); //con_timer_cb
#elif defined(USE_LOG_TIMER)
  /* Blink built-in NodeMCU LED every second */
  mgos_gpio_set_mode(LED_ALIVE_PIN, MGOS_GPIO_MODE_OUTPUT);
  //mgos_gpio_setup_output(LED_ALIVE_PIN, 0);
    mgos_set_timer(1000 /* ms */, MGOS_TIMER_REPEAT, log_timer_cb, NULL);
#endif

#ifdef USE_REPORT_TEMP
  /* Blink built-in ESP8266XX LED every second */
  mgos_gpio_set_mode(LED_REPORT_PIN, MGOS_GPIO_MODE_OUTPUT);

  /* Report temperature to AWS IoT Core every 5 mins */
  mgos_set_timer(300000, MGOS_TIMER_REPEAT, report_temperature, NULL);
#endif

  /* Publish to MQTT on button press */
  mgos_gpio_set_button_handler(0,
                               MGOS_GPIO_PULL_UP, MGOS_GPIO_INT_EDGE_NEG, 200,
                               button_cb, NULL);

#if defined(USE_DHT)
  if ((s_dht = mgos_dht_create(5, DHT11)) == NULL) {
    LOG(LL_INFO, ("Unable to initialize DHT11"));
  }
#endif

#if defined(USE_NET_CB)
  /* Network connectivity events */
  mgos_event_add_group_handler(MGOS_EVENT_GRP_NET, net_cb, NULL);
#endif

#if defined(USE_PORT_MQTT)
  mgos_mqtt_add_global_handler(ev_handler, NULL);
#endif

  return MGOS_APP_INIT_SUCCESS;
}
#endif//!USE_JS_AWS
