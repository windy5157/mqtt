/*******************************************************************************
 * Copyright (c) 2012, 2018 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *   https://www.eclipse.org/legal/epl-2.0/
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial contribution
 *    Guilherme Maciel Ferreira - add keep alive option
 *******************************************************************************/

#if !defined(__MQTT_CFG_DEFAULT_PARMS_H__)
#define __MQTT_CFG_DEFAULT_PARMS_H__

#include "mqtt_types.h"

#define MQTT_PARMS_STR_UID                          "efab1237efab1237"
#define MQTT_PARMS_STR_DEVICEID_FORMAT              "mpm%s"

#define MQTT_PARMS_STR_USER                         "amdin"
#define MQTT_PARMS_STR_PASS                         "admin"

#define MQTT_PARMS_STR_SERVER_USER                  "amdin"
#define MQTT_PARMS_STR_SERVER_PASS                  "admin"

#define MQTT_PARMS_STR_SERVER_DEVICEID              "mqttftserver1"

#define MQTT_PARMS_STR_HOST                         "172.16.22.51"
#define MQTT_PARMS_PORT                             1883

#define MQTT_PARMS_KEEPALIVE                        120

// 需要防止strtol整数越界
#define MQTT_PARMS_STR_LEVEL_START_OFFSET           12
#define MQTT_PARMS_STR_LEVEL_LEN                    8
#define MQTT_PARMS_STR2LONG_BASE_16                 16

#define MQTT_PARMS_LEVEL1_MOD_NUM                   16
#define MQTT_PARMS_LEVEL2_MOD_NUM                   1024

#endif
