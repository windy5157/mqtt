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
#include "./inc/mqtt_biz_info.h"

MQTT_BIZ_INFO mqttbizinfo;

void mqtt_biz_info_init(MQTT_BIZ_INFO* bizinfo)
{
	int rc;

	if (NULL == bizinfo)
	{
		return;
	
}
	
    bizinfo->mqtt_cli = NULL;
    bizinfo->tostop = 0;

	bizinfo->connstate = MQTT_CONN_STATE_IDLE;	
	
	mqtt_biz_cfg_init(&bizinfo->mqtt_cfg);
	redis_cfg_init(&bizinfo->redis_cfg);

	snprintf((const char*)&bizinfo->servurl, MQTT_PAHO_STR_SERV_URL_LEN_MAX,
		MQTT_PAHO_STR_SERV_URL_FORMAT, MQTT_PAHO_STR_SERV_TCP,
		(const char*)&bizinfo->mqtt_cfg.mqtt_host, bizinfo->mqtt_cfg.mqtt_port);

	snprintf(&bizinfo->subtopic, MQTT_PAHO_STR_SUB_TOPIC_LEN_MAX,
		MQTT_PAHO_STR_SUB_TOPIC_FORMAT, bizinfo->mqtt_cfg.mqtt_user);

    bizinfo->pausesem = Thread_create_sem(&rc);
}

void mqtt_biz_info_deinit(void)
{
}
