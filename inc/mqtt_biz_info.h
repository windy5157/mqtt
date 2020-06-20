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

#if !defined(__MQTT_BIZ_INFO_H__)
#define __MQTT_BIZ_INFO_H__

#include "Thread.h"
#include "mqtt_types.h"
#include "mqtt_cfg.h"
#include "hiredis.h"
#include "redis_cfg.h"

#define MQTT_PAHO_STR_SERV_URL_FORMAT       "%s://%s:%d"
#define MQTT_PAHO_STR_SERV_URL_LEN_MAX      64
#define MQTT_PAHO_STR_SERV_TCP              "tcp"
#define MQTT_PAHO_STR_SERV_IP               "172.16.22.51"

#define MQTT_PAHO_STR_PUB_TOPIC_LEN_MAX     64
#define MQTT_PAHO_STR_PUB_TOPIC_FORMAT      "/mpms/%s/%s"

#define MQTT_PAHO_STR_SUB_TOPIC_FORMAT      "/mpmp/#"
#define MQTT_PAHO_STR_SUB_TOPIC_LEN_MAX     64
#define MQTT_PAHO_QOS_0 					0

typedef enum {
    MQTT_CONN_STATE_IDLE = 0,   
    MQTT_CONN_STATE_FAILED,
    MQTT_CONN_STATE_LOST,
    MQTT_CONN_STATE_SUCCESSED,
} MQTT_CONN_STATE;

typedef struct {
	MQTT_BIZ_CFG	mqtt_cfg;
	MQTTClient 		mqtt_cli;	
	REDIS_CFG		redis_cfg;
	u8				servurl[MQTT_PAHO_STR_SERV_URL_LEN_MAX];
	u8				subtopic[MQTT_PAHO_STR_SUB_TOPIC_LEN_MAX];
	redisContext*   redis_cli;
	volatile int 	tostop;
	u32 			connstate;
	sem_type 		pausesem;
} MQTT_BIZ_INFO;

extern MQTT_BIZ_INFO mqttbizinfo;

extern void mqtt_biz_info_init(MQTT_BIZ_INFO* bizinfo);
extern void mqtt_biz_info_deinit(void);

#endif
