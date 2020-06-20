/*******************************************************************************
 * Copyright (c) 2012, 2020 IBM Corp.
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
 *******************************************************************************/
#include "./inc/Thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined(WIN32)
#include <unistd.h>
#else
#include <windows.h>
typedef struct {
	long    tv_sec;         /* seconds */
	long    tv_usec;        /* and microseconds */
} timeval;
#endif

#include "MQTTClient.h"
#include "./inc/mqtt_biz_info.h"
#include "./inc/hiredis.h"
#include "./inc/mqtt_ft_server_on_msg_arrvd.h"

#define MQTT_MSG_FIELD_DEIVCE               	0
#define MQTT_MSG_FIELD_LV1                  	1
#define MQTT_MSG_FIELD_LV2                  	2
#define MQTT_MSG_FIELD_UID                  	3
#define MQTT_MSG_FIELD_CMDCHAR              	4
#define MQTT_MSG_FIELD_MAX                  	5

#define MQTT_MSG_TOPIC_MAXLEN               	128
#define MQTT_MSG_f_MIN_LEN                  	9
#define MQTT_MSG_e_MIN_LEN                  	8

#define REDIS_MPM_CACHE_OK              		0
#define REDIS_MPM_CACHE_NOT_CONNECTED     		-1
#define REDIS_MPM_CACHE_EXEC_FAILED       		-2
#define REDIS_MPM_CACHE_EXPIRE_FAILED     		-3

#define REDIS_OPT_SETBIT                   	 	"SETBIT"
#define REDIS_OPT_EXPIRE                    	"EXPIRE %s %d"
#define REDIS_OPT_GET_PIECE_MAP             	"GET %s"

#define REDIS_HEX_OPT_SET                   	"SET"
#define REDIS_HEX_OPT_GET                   	"GET"
#define REDIS_HEX_OPT_EXPIRE                	"EXPIRE"

#define REDIS_MPM_CACHE_FILE_MAP_DEFAULT_LEN    256
#define REDIS_HEX_OPT_EXPIRE_TIMEOUT            300
#define REDIS_HEX_OPT_ARGC_COUNT                3

#define MQTT_CONN_DESTROY_TIMEOUT     		    3000
#define MQTT_CONN_PAUSE_TIMEOUT      		    3000
	
static volatile MQTTClient_deliveryToken deliveredtoken;

static void delays(u32 ns)
{
#if defined(_WIN32) || defined(_WIN64)
	Sleep(ns * 1000);
#else
	sleep(ns);
#endif
}

#if 0
defined(_WIN32) || defined(_WIN64)
redisContext *redisConnect(const char *ip, int port)
{
	return NULL;
}

void redisFree(redisContext *c)
{
}

void *redisCommand(redisContext *c, const char *format, ...)
{
	return NULL;
}

void *redisCommandArgv(redisContext *c, int argc, const char **argv, const size_t *argvlen)
{
	return NULL;
}

void freeReplyObject(void *reply)
{
}
#endif

static char file_map_defalut[REDIS_MPM_CACHE_FILE_MAP_DEFAULT_LEN] = {0};
	
static void delivered(void *context, MQTTClient_deliveryToken dt)
{   
    return;
}

static int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    char topic[MQTT_MSG_TOPIC_MAXLEN] = {0};
    int topicidx = 0;
    char topicchar = '\0';
    int slashcnt = 0;  
    char* fields[16] = {0};
    int redis_ret_status = REDIS_MPM_CACHE_OK;

    char *topicNametemp = topicName;
    int mqtttopicLen = 0 ;

	MQTT_BIZ_INFO* bizinfo = (MQTT_BIZ_INFO*)context;

	if (NULL == bizinfo)
    {     
        return;
    }
    
    if ((MQTT_MSG_TOPIC_MAXLEN - 1) < topicLen)
	{
        MQTTClient_freeMessage(&message);
        MQTTClient_free(topicName);
    
        return 1;
	}

    sprintf(topic, "%s", topicName);    
    mqtttopicLen = strlen(topic);
	
    while (topicidx < mqtttopicLen)
	{
        topicchar = *topicNametemp++;

        if ('/' == topicchar)
		{            
            fields[slashcnt] = &topic[0] + topicidx + 1;
            //printf("topic slash %d %s\r\n", slashcnt, fields[slashcnt]);
        
            slashcnt++;

            topic[topicidx] = '\0';
		}
		else
		{
            topic[topicidx] = topicchar;
        }

        topicidx++;
    }

    topic[mqtttopicLen] = '\0';
    
    if (MQTT_MSG_FIELD_MAX > slashcnt)
	{
        MQTTClient_freeMessage(&message);
        MQTTClient_free(topicName);
    
        return 1; 
	}


    switch (*fields[MQTT_MSG_FIELD_CMDCHAR])
	{
    case 'f':		
        if (message->payloadlen < MQTT_MSG_f_MIN_LEN)
		{
	        break;
		}

		mqtt_ft_server_on_msg_arrvd_f(context, fields[MQTT_MSG_FIELD_UID], message);		
        
        break;
    
    case 'F':
        break;

	case 'e':           
        if (message->payloadlen < MQTT_MSG_e_MIN_LEN) 
		{
            break;
		}

		mqtt_ft_server_on_msg_arrvd_e(context, fields[MQTT_MSG_FIELD_UID], message);
        
        break;

    default:
        break;
    }      
    
   
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    
    return 1;
}

static void connlost(void *context, char *cause)
{
    MQTT_BIZ_INFO* bizinfo = (MQTT_BIZ_INFO*)context;

	if (NULL == bizinfo)
    {     
        return;
    }

	bizinfo->connstate = MQTT_CONN_STATE_LOST;

	Thread_post_sem(bizinfo->pausesem);
}

static thread_return_type mqtt_file_server(void *parms)
{ 
	int rc;

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;    
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    MQTT_BIZ_INFO* bizinfo = (MQTT_BIZ_INFO*)parms;   

    if (NULL == bizinfo)
    {     
        return  0;
    }

    conn_opts.keepAliveInterval = bizinfo->mqtt_cfg.mqtt_keepalive;
    conn_opts.cleansession = 1;
    conn_opts.username = &bizinfo->mqtt_cfg.mqtt_user;
    conn_opts.password = &bizinfo->mqtt_cfg.mqtt_pass;

	printf("start thread\r\n");
	
    while (!bizinfo->tostop)
    {
        switch (bizinfo->connstate)
        {
        case MQTT_CONN_STATE_IDLE:
            MQTTClient_disconnect(bizinfo->mqtt_cli, MQTT_CONN_DESTROY_TIMEOUT);

            rc = MQTTClient_connect(bizinfo->mqtt_cli, &conn_opts);        
            if (MQTTCLIENT_SUCCESS != rc)
            {
				bizinfo->connstate = MQTT_CONN_STATE_FAILED;				
                
                break;
            }

            MQTTClient_subscribe(bizinfo->mqtt_cli, &bizinfo->subtopic, 0);
			bizinfo->connstate = MQTT_CONN_STATE_SUCCESSED;			
            
            break;
			
		case MQTT_CONN_STATE_SUCCESSED:
			Thread_wait_sem(bizinfo->pausesem, MQTT_CONN_PAUSE_TIMEOUT);
			
			break; 
       
        case MQTT_CONN_STATE_FAILED:
            Thread_wait_sem(bizinfo->pausesem, MQTT_CONN_PAUSE_TIMEOUT);
            
			bizinfo->connstate = MQTT_CONN_STATE_IDLE;
            
            break;

		case MQTT_CONN_STATE_LOST:
            MQTTClient_disconnect(bizinfo->mqtt_cli, MQTT_CONN_DESTROY_TIMEOUT);

			bizinfo->connstate = MQTT_CONN_STATE_IDLE;
            
            break;

        default:
            break;
        }
    }

    return 0;
}

int main(int argc, char* argv[])
{
	int ch; 
		
	mqtt_biz_info_init(&mqttbizinfo);
	
	mqtt_biz_redis_connect(&mqttbizinfo);

	MQTTClient_create(&mqttbizinfo.mqtt_cli, (const char*)&mqttbizinfo.servurl,
		(const char*)&mqttbizinfo.mqtt_cfg.device_id, MQTTCLIENT_PERSISTENCE_NONE, NULL);
	MQTTClient_setCallbacks(mqttbizinfo.mqtt_cli, &mqttbizinfo, connlost, msgarrvd, delivered);	
	
	Thread_start(&mqtt_file_server, (void *)&mqttbizinfo);

	do {	   
		ch = getchar();
	} while ((ch!='Q') && (ch != 'q')); 

	mqttbizinfo.tostop = 1;
	Thread_post_sem(mqttbizinfo.pausesem);

	delays(5);

	MQTTClient_destroy(&mqttbizinfo.mqtt_cli);	
	mqtt_biz_info_deinit();
		
	return 0;
}