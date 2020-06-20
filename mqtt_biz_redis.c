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
#include "./inc/mqtt_biz_redis.h"
#include "./inc/mqtt_biz_info.h"

#define MQTT_MSG_FIELD_DEIVCE               	0
#define MQTT_MSG_FIELD_LV1                  	1
#define MQTT_MSG_FIELD_LV2                  	2
#define MQTT_MSG_FIELD_UID                  	3
#define MQTT_MSG_FIELD_CMDCHAR              	4
#define MQTT_MSG_FIELD_MAX                  	5

#define REDIS_OPT_SETBIT                   	 	"SETBIT"
#define REDIS_OPT_EXPIRE                    	"EXPIRE %s %d"
#define REDIS_OPT_GET_PIECE_MAP             	"GET %s"

#define REDIS_HEX_OPT_SET                   	"SET"
#define REDIS_HEX_OPT_GET                   	"GET"
#define REDIS_HEX_OPT_EXPIRE                	"EXPIRE"
#define REDIS_HEX_OPT_SETBIT                   	"SETBIT"
#define REDIS_HEX_OPT_SETBIT_PIECEIDX_LEN       16

#define REDIS_MPM_CACHE_FILE_MAP_DEFAULT_LEN    16
#define REDIS_HEX_OPT_EXPIRE_TIMEOUT            300
#define REDIS_HEX_OPT_ARGC_COUNT                3

#define MQTT_CONN_DESTROY_TIMEOUT         		3000
#define MQTT_CONN_PAUSE_TIMEOUT           		3000

#define REDIS_MPM_CACHE_OK              		0
#define REDIS_MPM_CACHE_NOT_CONNECTED     		-1
#define REDIS_MPM_CACHE_EXEC_FAILED       		-2
#define REDIS_MPM_CACHE_EXPIRE_FAILED     		-3

static char file_map_defalut[REDIS_MPM_CACHE_FILE_MAP_DEFAULT_LEN] = {0};

redisContext *mqtt_biz_redis_connect(MQTT_BIZ_INFO*       parms)
{
 	MQTT_BIZ_INFO* bizinfo = (MQTT_BIZ_INFO*)parms;
	
	if (NULL == parms)
	{
        return NULL;
    }		
	
    if (bizinfo->redis_cli)
	{
        redisFree(bizinfo->redis_cli);
        bizinfo->redis_cli = NULL;    
    }

#if 0
	bizinfo->redis_cli = redisConnect(bizinfo->redis_cfg.redis_host, bizinfo->redis_cfg.redis_port);
	if (NULL == bizinfo->redis_cli)
	{
		printf("Error: %s\n", bizinfo->redis_cli->errstr);
	}
#endif
	struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    bizinfo->redis_cli = redisConnectWithTimeout(bizinfo->redis_cfg.redis_host, bizinfo->redis_cfg.redis_port, timeout); 
	
	return bizinfo->redis_cli;
}

void mqtt_biz_redis_disconnect(MQTT_BIZ_INFO*        parms)
{
	MQTT_BIZ_INFO* bizinfo = (MQTT_BIZ_INFO*)parms;
	
	if (NULL == parms)
	{
        return;
    }

    if (bizinfo->redis_cli)
	{
        redisFree(bizinfo->redis_cli);
        bizinfo->redis_cli = NULL;
    }
}

int mqtt_biz_redis_cache_hex_buffer (MQTT_BIZ_INFO*          parms, const char *key, unsigned char *hexbuf,  unsigned int hexbuflen)
{
	MQTT_BIZ_INFO* bizinfo = (MQTT_BIZ_INFO*)parms;
	
    const char *argv[REDIS_HEX_OPT_ARGC_COUNT] = {0};
    size_t argvlen[REDIS_HEX_OPT_ARGC_COUNT] = {0};
    redisReply *reply = NULL;

	if (NULL == parms)
	{		
        return REDIS_MPM_CACHE_NOT_CONNECTED;
    }

    if (!bizinfo->redis_cli)
	{
		        return REDIS_MPM_CACHE_NOT_CONNECTED;
	}
    	
    argv[0] = REDIS_HEX_OPT_SET;
    argvlen[0] = strlen(REDIS_HEX_OPT_SET);

    argv[1] = key;
    argvlen[1] = strlen(key);

    argv[2] = hexbuf;
    argvlen[2] = hexbuflen;	
	
    reply = (redisReply *)redisCommandArgv(bizinfo->redis_cli, REDIS_HEX_OPT_ARGC_COUNT, argv, argvlen);
    if (!reply)
	{		
        return REDIS_MPM_CACHE_EXEC_FAILED;     
    }

    freeReplyObject(reply);
    reply = NULL;
    
    reply = (redisReply *)redisCommand(bizinfo->redis_cli, REDIS_OPT_EXPIRE, key, REDIS_HEX_OPT_EXPIRE_TIMEOUT);
    if (!reply)
	{
        return REDIS_MPM_CACHE_EXPIRE_FAILED;       
    }

    freeReplyObject(reply);
    reply = NULL;

    return REDIS_MPM_CACHE_OK;  
}

int mqtt_biz_redis_cache_file_init (MQTT_BIZ_INFO*          parms, const char *key, size_t fileidx)
{
	MQTT_BIZ_INFO* bizinfo = (MQTT_BIZ_INFO*)parms;

    char keytemp[64] = {0};

	if (NULL == parms)
	{
        return REDIS_MPM_CACHE_NOT_CONNECTED;
    }
    
    if (!bizinfo->redis_cli)
	{
        return REDIS_MPM_CACHE_NOT_CONNECTED;   
    }

    sprintf(keytemp, "%s:%ld:m", key, fileidx);
    
    redisReply *reply = (redisReply *)redisCommand(bizinfo->redis_cli, "DEL %s", keytemp);
    if (!reply)
	{
        return REDIS_MPM_CACHE_EXEC_FAILED;     
    }

    return mqtt_biz_redis_cache_hex_buffer(parms, keytemp, &file_map_defalut, REDIS_MPM_CACHE_FILE_MAP_DEFAULT_LEN);
}

int mqtt_biz_redis_cache_file_piece (MQTT_BIZ_INFO*          parms, const char *key, size_t fileidx, size_t pieceidx, unsigned char *hexbuf,  unsigned int hexbuflen)
{
	MQTT_BIZ_INFO* bizinfo = (MQTT_BIZ_INFO*)parms;

    char keytemp[64] = {0};
    int retvalue = REDIS_MPM_CACHE_OK;
	
	if (NULL == parms)
	{
        return REDIS_MPM_CACHE_NOT_CONNECTED;
    }
    
    if (!bizinfo->redis_cli)
 	{
        return REDIS_MPM_CACHE_NOT_CONNECTED;
    }
	
    sprintf(keytemp, "%s:%lu:%lu", key, fileidx, pieceidx);
    
    retvalue = mqtt_biz_redis_cache_hex_buffer(parms, keytemp, hexbuf, hexbuflen);
    if (retvalue)
    {        
        return retvalue;    
    }   

    sprintf(keytemp, "%s:%lu:m", key, fileidx);  

    return mqtt_biz_redis_set_cache_file_piece_map(parms, keytemp, pieceidx);
}

int mqtt_biz_redis_set_cache_file_piece_map (MQTT_BIZ_INFO*            parms, const char *key, size_t pieceidx)
{
	const char *setbit_parms[4];
	size_t setbit_parms_len[4];	
	redisReply *reply = NULL;
	char *pieceidx_ascii[REDIS_HEX_OPT_SETBIT_PIECEIDX_LEN] = {0};
	MQTT_BIZ_INFO* bizinfo = (MQTT_BIZ_INFO*)parms;

	if (NULL == parms)
	{
		return REDIS_MPM_CACHE_NOT_CONNECTED;
	}

    if (!bizinfo->redis_cli)
	{
        return REDIS_MPM_CACHE_NOT_CONNECTED;
    }
	
	setbit_parms[0] = REDIS_HEX_OPT_SETBIT;
	setbit_parms_len[0] = strlen(REDIS_HEX_OPT_SETBIT);

	setbit_parms[1] = key;
	setbit_parms_len[1] = strlen(key);

	sprintf(&pieceidx_ascii, "%ld", pieceidx);
	setbit_parms[2] = &pieceidx_ascii;
	setbit_parms_len[2] = strlen(&pieceidx_ascii);

	setbit_parms[3] = "1";
	setbit_parms_len[3] = strlen("1");

	reply = (redisReply *)redisCommandArgv(bizinfo->redis_cli, sizeof(setbit_parms) / sizeof(setbit_parms[0]), setbit_parms, setbit_parms_len);
    if (!reply)
	{
        return REDIS_MPM_CACHE_EXEC_FAILED;     
    }

	freeReplyObject(reply);	
	reply = NULL;

    return REDIS_MPM_CACHE_OK;
}

char *mqtt_biz_redis_get_cache_file_piece_map (MQTT_BIZ_INFO*            parms, const char *key)
{
	MQTT_BIZ_INFO* bizinfo = (MQTT_BIZ_INFO*)parms;

    redisReply *reply = NULL;

	if (NULL == parms)
	{
		return REDIS_MPM_CACHE_NOT_CONNECTED;
	}

	if (!bizinfo->redis_cli)
	{
		return REDIS_MPM_CACHE_NOT_CONNECTED;
	}
    
    reply = (redisReply *)redisCommand(bizinfo->redis_cli, REDIS_OPT_GET_PIECE_MAP, key);
    if (!reply)
	{
        return REDIS_MPM_CACHE_EXPIRE_FAILED;       
    }

    if (reply->str)
    {
    	freeReplyObject(reply);
    	reply = NULL;
    }

    return REDIS_MPM_CACHE_OK;
}
