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

#if !defined(__REDIS_CFG_H__)
#define __REDIS_CFG_H__

#include "mqtt_types.h"
#include "hiredis.h"
#include "redis_cfg_default_parms.h"

typedef struct
{
	redisContext *redis;
	u8      redis_user[REDIS_CFG_STR_USER_LEN];
	u8      redis_pass[REDIS_CFG_STR_PASS_LEN];
	u8      redis_host[REDIS_CFG_STR_HOST_LEN];
	u32     redis_port;	
} REDIS_CFG;

extern void redis_cfg_init(REDIS_CFG*);

#endif
