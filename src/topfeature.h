#ifndef TOPFEATURE_H
#define TOPFEATURE_H

#define USE_DHT
#undef USE_DHT

#if defined(USE_DHT)
    #define USE_REPORT_TEMP
    #undef USE_REPORT_TEMP
#endif

#define USE_NET_CB
#undef USE_NET_CB

#if !defined(USE_NET_CB)
    #define USE_LOG_TIMER
    //#undef USE_LOG_TIMER
#endif

#define USE_PORT_MQTT
//#undef USE_PORT_MQTT
#if defined(USE_PORT_MQTT)
    #define USE_LOG_TIMER
    //#undef USE_LOG_TIMER
#endif

#define USE_JS_AWS_TEST
#undef USE_JS_AWS_TEST

#endif