/* libsmp
 * Copyright (C) 2017 Actronika SAS
 *     Author: Aurélien Zanelli <aurelien.zanelli@actronika.com>
 */

#include "libtup.h"
#include <errno.h>
#include <stdio.h>

void tup_message_clear(TupMessage *message)
{
    smp_message_clear(message);
}

void tup_message_init_ack(TupMessage *message, TupMessageType cmd)
{
    smp_message_init(message, TUP_MESSAGE_ACK);
    smp_message_set_uint32(message, 0, cmd);
}

int tup_message_parse_ack(TupMessage *message, TupMessageType *cmd)
{
    uint32_t cmd_id;
    int ret;

    if (smp_message_get_msgid(message) != TUP_MESSAGE_ACK)
        return -EBADMSG;

    ret = smp_message_get_uint32(message, 0, &cmd_id);
    if (ret < 0)
        return ret;

    *cmd = (TupMessageType) cmd_id;
    return 0;
}

void tup_message_init_error(TupMessage *message, TupMessageType cmd,
        uint32_t error)
{
    smp_message_init(message, TUP_MESSAGE_ERROR);
    smp_message_set_uint32(message, 0, cmd);
    smp_message_set_uint32(message, 1, error);
}

int tup_message_parse_error(TupMessage *message, TupMessageType *cmd,
        uint32_t *error)
{
    uint32_t cmd_id;
    int ret;

    if (smp_message_get_msgid(message) != TUP_MESSAGE_ERROR)
        return -EBADMSG;

    ret = smp_message_get(message, 0, SMP_TYPE_UINT32, &cmd_id,
            1, SMP_TYPE_UINT32, error, -1);
    if (ret < 0)
        return ret;

    *cmd = cmd_id;
    return 0;
}

void tup_message_init_load(TupMessage *message, uint8_t effect_id,
        uint16_t bank_id)
{
    smp_message_init(message, TUP_MESSAGE_CMD_LOAD);
    smp_message_set_uint8(message, 0, effect_id);
    smp_message_set_uint16(message, 1, bank_id);
}

int tup_message_parse_load(TupMessage *message, uint8_t *effect_id,
        uint16_t *bank_id)
{
    if (smp_message_get_msgid(message) != TUP_MESSAGE_CMD_LOAD)
        return -EBADMSG;

    return smp_message_get(message, 0, SMP_TYPE_UINT8, effect_id,
            1, SMP_TYPE_UINT16, bank_id, -1);
}

void tup_message_init_play(TupMessage *message, uint8_t effect_id)
{
    smp_message_init(message, TUP_MESSAGE_CMD_PLAY);
    smp_message_set_uint8(message, 0, effect_id);
}

int tup_message_parse_play(TupMessage *message, uint8_t *effect_id)
{
    if (smp_message_get_msgid(message) != TUP_MESSAGE_CMD_PLAY)
        return -EBADMSG;

    return smp_message_get_uint8(message, 0, effect_id);
}

void tup_message_init_stop(TupMessage *message, uint8_t effect_id)
{
    smp_message_init(message, TUP_MESSAGE_CMD_STOP);
    smp_message_set_uint8(message, 0, effect_id);
}

int tup_message_parse_stop(TupMessage *message, uint8_t *effect_id)
{
    if (smp_message_get_msgid(message) != TUP_MESSAGE_CMD_STOP)
        return -EBADMSG;

    return smp_message_get_uint8(message, 0, effect_id);
}

void tup_message_init_get_version(TupMessage *message)
{
    smp_message_init(message, TUP_MESSAGE_CMD_GET_VERSION);
}

void tup_message_init_get_parameter(TupMessage *message, uint8_t effect_id,
        uint8_t parameter_id)
{
    smp_message_init(message, TUP_MESSAGE_CMD_GET_PARAMETER);
    smp_message_set_uint8(message, 0, effect_id);
    smp_message_set_uint8(message, 1, parameter_id);
}

int tup_message_parse_get_parameter(TupMessage *message, uint8_t *effect_id,
        uint8_t *parameter_id)
{
    if (smp_message_get_msgid(message) != TUP_MESSAGE_CMD_GET_PARAMETER)
        return -EBADMSG;

    return smp_message_get(message, 0, SMP_TYPE_UINT8, effect_id,
            1, SMP_TYPE_UINT8, parameter_id, -1);
}

void tup_message_init_set_parameter(TupMessage *message, uint8_t effect_id,
        uint8_t parameter_id, uint32_t parameter_value)
{
    smp_message_init(message, TUP_MESSAGE_CMD_SET_PARAMETER);
    smp_message_set_uint8(message, 0, effect_id);
    smp_message_set_uint8(message, 1, parameter_id);
    smp_message_set_uint32(message, 2, parameter_value);
}

int tup_message_parse_set_parameter(TupMessage *message, uint8_t *effect_id,
        uint8_t *parameter_id, uint32_t *parameter_value)
{
    if (smp_message_get_msgid(message) != TUP_MESSAGE_CMD_SET_PARAMETER)
        return -EBADMSG;

    return smp_message_get(message, 0, SMP_TYPE_UINT8, effect_id,
            1, SMP_TYPE_UINT8, parameter_id,
            2, SMP_TYPE_UINT32, parameter_value, -1);
}

void tup_message_init_bind_effect(TupMessage *message, uint8_t effect_id,
        unsigned int binding_flags)
{
    smp_message_init(message, TUP_MESSAGE_CMD_BIND_EFFECT);
    smp_message_set_uint8(message, 0, effect_id);
    smp_message_set_uint8(message, 1, binding_flags);
}

int tup_message_parse_bind_effect(TupMessage *message, uint8_t *effect_id,
        unsigned int *binding_flags)
{
    uint8_t flags = 0;
    int ret;

    if (smp_message_get_msgid(message) != TUP_MESSAGE_CMD_BIND_EFFECT)
        return -EBADMSG;

    ret = smp_message_get(message, 0, SMP_TYPE_UINT8, effect_id,
            1, SMP_TYPE_UINT8, &flags, -1);

    *binding_flags = flags;
    return ret;
}

void tup_message_init_get_sensor_value(TupMessage *message, uint8_t sensor_id)
{
    smp_message_init(message, TUP_MESSAGE_CMD_GET_SENSOR_VALUE);
    smp_message_set_uint8(message, 0, sensor_id);
}

int tup_message_parse_get_sensor_value(TupMessage *message, uint8_t *sensor_id)
{
    if (smp_message_get_msgid(message) != TUP_MESSAGE_CMD_GET_SENSOR_VALUE)
        return -EBADMSG;

    return smp_message_get_uint8(message, 0, sensor_id);
}

void tup_message_init_set_sensor_value(TupMessage *message, uint8_t sensor_id,
        uint16_t sensor_value)
{
    smp_message_init(message, TUP_MESSAGE_CMD_SET_SENSOR_VALUE);
    smp_message_set_uint8(message, 0, sensor_id);
    smp_message_set_uint16(message, 1, sensor_value);
}

int tup_message_parse_set_sensor_value(TupMessage *message, uint8_t *sensor_id,
        uint16_t *sensor_value)
{
    if (smp_message_get_msgid(message) != TUP_MESSAGE_CMD_SET_SENSOR_VALUE)
        return -EBADMSG;

    return smp_message_get(message, 0, SMP_TYPE_UINT8, sensor_id,
            1, SMP_TYPE_UINT16, sensor_value, -1);
}

void tup_message_init_resp_version(TupMessage *message, const char *version)
{
    smp_message_init(message, TUP_MESSAGE_RESP_VERSION);
    smp_message_set_cstring(message, 0, version);
}

int tup_message_parse_resp_version(TupMessage *message, const char **version)
{
    if (smp_message_get_msgid(message) != TUP_MESSAGE_RESP_VERSION)
        return -EBADMSG;

    return smp_message_get_cstring(message, 0, version);
}

void tup_message_init_resp_parameter(TupMessage *message, uint8_t effect_id,
        uint8_t parameter_id, uint32_t parameter_value)
{
    smp_message_init(message, TUP_MESSAGE_RESP_PARAMETER);
    smp_message_set_uint8(message, 0, effect_id);
    smp_message_set_uint8(message, 1, parameter_id);
    smp_message_set_uint32(message, 2, parameter_value);
}

int tup_message_parse_resp_parameter(TupMessage *message, uint8_t *effect_id,
        uint8_t *parameter_id, uint32_t *parameter_value)
{
    if (smp_message_get_msgid(message) != TUP_MESSAGE_RESP_PARAMETER)
        return -EBADMSG;

    return smp_message_get(message, 0, SMP_TYPE_UINT8, effect_id,
            1, SMP_TYPE_UINT8, parameter_id,
            2, SMP_TYPE_UINT32, parameter_value, -1);
}

void tup_message_init_resp_sensor(TupMessage *message, uint8_t sensor_id,
        uint16_t sensor_value)
{
    smp_message_init(message, TUP_MESSAGE_RESP_SENSOR);
    smp_message_set_uint8(message, 0, sensor_id);
    smp_message_set_uint16(message, 1, sensor_value);
}

int tup_message_parse_resp_sensor(TupMessage *message, uint8_t *sensor_id,
        uint16_t *sensor_value)
{
    if (smp_message_get_msgid(message) != TUP_MESSAGE_RESP_SENSOR)
        return -EBADMSG;

    return smp_message_get(message, 0, SMP_TYPE_UINT8, sensor_id,
            1, SMP_TYPE_UINT16, sensor_value, -1);
}
