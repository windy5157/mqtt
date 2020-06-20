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
#include "./inc/mqtt_ft_server_on_msg_arrvd.h"
#include "./inc/mqtt_biz_redis.h"

#define REDIS_MPM_CACHE_OK              		0
#define REDIS_MPM_CACHE_NOT_CONNECTED     		-1
#define REDIS_MPM_CACHE_EXEC_FAILED       		-2
#define REDIS_MPM_CACHE_EXPIRE_FAILED     		-3
#define MQTT_MSG_TOPIC_MAXLEN               	128

void mqtt_ft_server_on_msg_arrvd_f(void *context, char *uid, MQTTClient_message *message)
{
	MQTT_BIZ_INFO* bizinfo = (MQTT_BIZ_INFO*)context;
	char rediskey[MQTT_MSG_TOPIC_MAXLEN] = {0};  
	unsigned short fileidx = 0;
    unsigned short pieceidx = 0;

	if (NULL == bizinfo)
    {     
        return;
    }	

    sprintf(&rediskey, "mpmp:%s", uid); 
	
    fileidx = 0;
    fileidx += *(((char*)message->payload) + 2);
    fileidx += (*(((char*)message->payload) + 3)) << 8;

    pieceidx = 0;
    pieceidx += *(((char*)message->payload) + 4);
    pieceidx += (*(((char*)message->payload) + 5)) << 8;      
    
    int redis_ret_status = mqtt_biz_redis_cache_file_piece(context, &rediskey, fileidx, pieceidx, ((unsigned char*)message->payload) + 8, message->payloadlen - 8);
	if (REDIS_MPM_CACHE_OK != redis_ret_status) 
	{
        // 错误处理
    }  
	
    return;
}

int mqtt_ft_server_on_msg_arrvd_e(void *context, char *uid, MQTTClient_message *message)
{
#if 0
	MQTT_BIZ_INFO* bizinfo = (MQTT_BIZ_INFO*)context;
	char rediskey[MQTT_MSG_TOPIC_MAXLEN] = {0};  
	unsigned short fileidx = 0;
	unsigned short pieceidx = 0;
    unsigned short pieceidxbegin = 0;
	unsigned short pieceidxend = 0;
	u8 pubmsgbody[MQTT_FT_SEND_e_MSG_BODY_LEN] = {0};

	if (NULL == bizinfo)
    {     
        return;
    }	

    sprintf(rediskey, "mpmp:%s", uid);        

    fileidx = 0;
    fileidx += *(((char*)message->payload) + 2);
    fileidx += (*(((char*)message->payload) + 3)) << 8;

    pieceidxbegin = 0;
    pieceidxbegin += *(((char*)message->payload) + 4);
    pieceidxbegin += (*(((char*)message->payload) + 5)) << 8;  

	pieceidxend = 0;
    pieceidxend += *(((char*)message->payload) + 4);
    pieceidxend += (*(((char*)message->payload) + 5)) << 8;  
	
	pubmsgbody[0] = (MQTT_FT_REDIS_EXPIRE & 0x00ff);
	pubmsgbody[1] = (MQTT_FT_REDIS_EXPIRE & 0xff00) >> 8;
	pubmsgbody[2] = (fileidx & 0x00ff);		
	pubmsgbody[3] = (fileidx & 0xff00) >> 8;

	for (pieceidx = pieceidxbegin; pieceidx < pieceidxend; pieceidx++)
	{
		
	
}
	
	pubmsgbody[4] = (0 & 0x00ff); 	
	pubmsgbody[5] = (0 & 0xff00) >> 8;
	pubmsgbody[6] = (pieceincurfile & 0x00ff); 	
	pubmsgbody[7] = (pieceincurfile & 0xff00) >> 8;
	
	pubmsg.payload = &pubmsgbody;
    pubmsg.payloadlen = MQTT_FT_SEND_e_MSG_BODY_LEN;
    pubmsg.qos = MQTT_PAHO_QOS_0;
    pubmsg.retained = 0;
	
	snprintf(&pubtopic, MQTT_PAHO_STR_PUB_TOPIC_LEN_MAX, MQTT_PAHO_STR_PUB_TOPIC_FORMAT, 
		uid, "e");
	
	MQTTClient_publishMessage(clientinfo->client, (const char*)&pubtopic, &pubmsg, NULL);
#endif
	return 0;
}