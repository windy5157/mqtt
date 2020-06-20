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
#include "./inc/redis_cfg.h"

#define REDIS_PARMS_STR_USER                        "amdin"
#define REDIS_PARMS_STR_PASS                        "admin"

#define REDIS_PARMS_STR_HOST_LEN		           	64
#define REDIS_PARMS_STR_USER_LEN           			32
#define REDIS_PARMS_STR_PASS_LEN       			  	32

void redis_cfg_init(REDIS_CFG* rediscfg)
{

	if (NULL == rediscfg)
	{
		return;
	}	

	rediscfg->redis = NULL;
	
	strncpy(rediscfg->redis_host, REDIS_PARMS_STR_HOST, (REDIS_PARMS_STR_HOST_LEN - 1));
	strncpy(rediscfg->redis_user, REDIS_PARMS_STR_USER, (REDIS_PARMS_STR_USER_LEN - 1));
	strncpy(rediscfg->redis_pass, REDIS_PARMS_STR_PASS, (REDIS_PARMS_STR_PASS_LEN - 1));
	rediscfg->redis_port = REDIS_PARMS_PORT;
}
