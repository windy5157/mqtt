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

#if !defined(__MQTT_CFG_H__)
#define __MQTT_CFG_H__

#include "mqtt_types.h"
#include "mqtt_cfg_default_parms.h"

#define MQTT_CFG_STR_DEVICEID_LEN       	32
#define MQTT_CFG_STR_HOST_LEN           	64
#define MQTT_CFG_STR_USER_LEN           	32
#define MQTT_CFG_STR_PASS_LEN           	32

typedef struct{
    u8     device_id[MQTT_CFG_STR_DEVICEID_LEN];
	u8     mqtt_user[MQTT_CFG_STR_USER_LEN];
	u8     mqtt_pass[MQTT_CFG_STR_PASS_LEN];
	u8     mqtt_host[MQTT_CFG_STR_HOST_LEN];
	u32    mqtt_port;
	u32    mqtt_keepalive;
	u16    lv1;
	u16    lv2;	
	u32    pad;
} MQTT_CLI_CFG;

typedef struct{
	u8     device_id[MQTT_CFG_STR_DEVICEID_LEN];
	u8     mqtt_user[MQTT_CFG_STR_USER_LEN];
	u8     mqtt_pass[MQTT_CFG_STR_PASS_LEN];
	u8     mqtt_host[MQTT_CFG_STR_HOST_LEN];
	u32    mqtt_port;
	u32    mqtt_keepalive;
} MQTT_BIZ_CFG;

extern void mqtt_cli_cfg_init(MQTT_CLI_CFG* cfg);
extern void mqtt_biz_cfg_init(MQTT_BIZ_CFG* cfg);

#endif
