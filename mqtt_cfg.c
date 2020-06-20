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
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "./inc/mqtt_cfg.h"

void mqtt_cli_cfg_init(MQTT_CLI_CFG* cfg)
{
	char temp[64] = {0};
	u32 level = 0;
	char *endptr = NULL;
	
	if (NULL == cfg)
	{
		return;
	}
	
	cfg->mqtt_port = MQTT_PARMS_PORT;
	cfg->mqtt_keepalive = MQTT_PARMS_KEEPALIVE;	
	strncpy(cfg->mqtt_host, MQTT_PARMS_STR_HOST, (MQTT_CFG_STR_HOST_LEN - 1));	

	strncpy(cfg->mqtt_user, MQTT_PARMS_STR_USER, (MQTT_CFG_STR_USER_LEN - 1));
	strncpy(cfg->mqtt_pass, MQTT_PARMS_STR_PASS, (MQTT_CFG_STR_PASS_LEN - 1));
	snprintf(cfg->device_id, MQTT_CFG_STR_DEVICEID_LEN, MQTT_PARMS_STR_DEVICEID_FORMAT, MQTT_PARMS_STR_UID);

	strncpy(temp, &cfg->device_id[MQTT_PARMS_STR_LEVEL_START_OFFSET], MQTT_PARMS_STR_LEVEL_LEN);
	level = strtol(temp, &endptr, MQTT_PARMS_STR2LONG_BASE_16);
	if (NULL == endptr)
	{
		return;
	}

	cfg->lv1 = (level % MQTT_PARMS_LEVEL1_MOD_NUM);
	cfg->lv2 = (level % MQTT_PARMS_LEVEL2_MOD_NUM);
}

void mqtt_biz_cfg_init(MQTT_BIZ_CFG* cfg)
{
	char temp[64] = {0};
	u32 level = 0;
	char *endptr = NULL;
	
	if (NULL == cfg)
	{
		return;
	}

	strncpy(cfg->device_id, MQTT_PARMS_STR_SERVER_DEVICEID, (MQTT_CFG_STR_DEVICEID_LEN - 1));
	strncpy(cfg->mqtt_user, MQTT_PARMS_STR_SERVER_USER, (MQTT_CFG_STR_USER_LEN - 1));
	strncpy(cfg->mqtt_pass, MQTT_PARMS_STR_SERVER_PASS, (MQTT_CFG_STR_PASS_LEN - 1));
	
	strncpy(cfg->mqtt_host, MQTT_PARMS_STR_HOST, (MQTT_CFG_STR_HOST_LEN - 1));	
	cfg->mqtt_port = MQTT_PARMS_PORT;
	cfg->mqtt_keepalive = MQTT_PARMS_KEEPALIVE;
}
