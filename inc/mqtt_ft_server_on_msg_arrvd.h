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

#if !defined(__MQTT_FT_SERVER_ON_MSG_ARRVD_H__)
#define __MQTT_FT_SERVER_ON_MSG_ARRVD_H__

#include "mqtt_types.h"
#include "MQTTClient.h"

extern void mqtt_ft_server_on_msg_arrvd_f(void *context, char *uid, MQTTClient_message *message);
extern int mqtt_ft_server_on_msg_arrvd_e(void *context, char *uid, MQTTClient_message *message);

#endif
