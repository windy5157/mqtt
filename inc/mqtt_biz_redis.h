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

#if !defined(__MQTT_BIZ_REDIS_H__)
#define __MQTT_BIZ_REDIS_H__

#include "Thread.h"
#include "mqtt_types.h"
#include "mqtt_biz_info.h"

extern redisContext *mqtt_biz_redis_connect(MQTT_BIZ_INFO*       parms);
extern void mqtt_biz_redis_disconnect(MQTT_BIZ_INFO*        parms);
extern int mqtt_biz_redis_cache_hex_buffer (MQTT_BIZ_INFO*          parms, const char *key, unsigned char *hexbuf,  unsigned int hexbuflen);
extern int mqtt_biz_redis_cache_file_init (MQTT_BIZ_INFO* parms, const char *key, size_t fileidx);
extern int mqtt_biz_redis_cache_file_piece (MQTT_BIZ_INFO* parms, const char *key, size_t fileidx, size_t pieceidx, unsigned char *hexbuf,  unsigned int hexbuflen);
extern int mqtt_biz_redis_set_cache_file_piece_map (MQTT_BIZ_INFO* parms, const char *key, size_t pieceidx);
extern char *mqtt_biz_redis_get_cache_file_piece_map (MQTT_BIZ_INFO* parms, const char *key);
extern char *mqtt_biz_redis_get_cache_file_piece_map (MQTT_BIZ_INFO*            parms, const char *key);

#endif
