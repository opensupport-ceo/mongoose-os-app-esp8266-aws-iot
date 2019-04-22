/*
 * Copyright (c) 2014-2019 OWL Software Limited
 * All rights reserved
 * 
 * Author: Jaehong Park, jaehong1972@gmail.com
 * 
 * Description: The most top feature definition header file.
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