/* libsmp
 * Copyright (C) 2017 Actronika SAS
 *     Author: Aurélien Zanelli <aurelien.zanelli@actronika.com>
 */

#include "libtup.h"
#include <errno.h>
#include <stdarg.h>

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

int tup_message_init_get_parameter(TupMessage *message, uint8_t effect_id,
        int parameter_id, ...)
{
    va_list ap;
    int ret;

    va_start(ap, parameter_id);
    ret = tup_message_init_get_parameter_valist(message, effect_id,
            parameter_id, ap);
    va_end(ap);

    return ret;
}

int tup_message_init_get_parameter_valist(TupMessage *message,
        uint8_t effect_id, int parameter_id, va_list varargs)
{
    int ret;
    int i;

    smp_message_init(message, TUP_MESSAGE_CMD_GET_PARAMETER);
    smp_message_set_uint8(message, 0, effect_id);

    for (i = 1; parameter_id != -1; i++) {
        ret = smp_message_set_uint8(message, i, parameter_id);
        if (ret < 0)
            return ret;

        parameter_id = va_arg(varargs, int);
    }

    return 0;
}

void tup_message_init_get_parameter_simple(TupMessage *message,
        uint8_t effect_id, uint8_t parameter_id)
{
    tup_message_init_get_parameter(message, effect_id, parameter_id, -1);
}

int tup_message_init_get_parameter_array(TupMessage *message,
        uint8_t effect_id, uint8_t *parameter_ids, size_t n_parameters)
{
    int ret;
    int i;

    smp_message_init(message, TUP_MESSAGE_CMD_GET_PARAMETER);
    smp_message_set_uint8(message, 0, effect_id);

    for (i = 0; i < n_parameters; i++) {
        ret = smp_message_set_uint8(message, i + 1, parameter_ids[i]);
        if (ret < 0)
            return ret;
    }

    return 0;
}

/* returns the number of parameters, -1 on error */
int tup_message_parse_get_parameter(TupMessage *message, uint8_t *effect_id,
        uint8_t *parameter_ids, size_t size)
{
    int n_params = 0;
    int ret;
    int i;

    if (smp_message_get_msgid(message) != TUP_MESSAGE_CMD_GET_PARAMETER)
        return -EBADMSG;

    ret = smp_message_get_uint8(message, 0, effect_id);
    if (ret < 0)
        return ret;

    n_params = smp_message_n_args(message) - 1;
    if (size < n_params) {
        /* array too small */
        return -ENOMEM;
    }

    for (i = 0; i < n_params; i++) {
        ret = smp_message_get_uint8(message, i + 1, &parameter_ids[i]);
        if (ret < 0) {
            /* should not happen but... */
            return ret;
        }
    }

    return n_params;
}

int tup_message_init_set_parameter(TupMessage *message, uint8_t effect_id,
        int parameter_id, uint32_t parameter_value, ...)
{
    va_list ap;
    int ret;

    va_start(ap, parameter_value);
    ret = tup_message_init_set_parameter_valist(message, effect_id,
            parameter_id, parameter_value, ap);
    va_end(ap);

    return ret;
}

int tup_message_init_set_parameter_valist(TupMessage *message,
        uint8_t effect_id, int parameter_id, uint32_t parameter_value,
        va_list varargs)
{
    int ret;
    int i;

    smp_message_init(message, TUP_MESSAGE_CMD_SET_PARAMETER);
    smp_message_set_uint8(message, 0, effect_id);

    for (i = 1; parameter_id != -1; i += 2) {
        ret = smp_message_set_uint8(message, i, parameter_id);
        if (ret < 0)
            return ret;

        ret = smp_message_set_uint32(message, i + 1, parameter_value);
        if (ret < 0)
            return ret;

        parameter_id = va_arg(varargs, int);
        parameter_value = va_arg(varargs, uint32_t);
    }

    return 0;
}

void tup_message_init_set_parameter_simple(TupMessage *message,
        uint8_t effect_id, uint8_t parameter_id, uint32_t parameter_value)
{
    tup_message_init_set_parameter(message, effect_id, parameter_id,
            parameter_value, -1);
}

int tup_message_init_set_parameter_array(TupMessage *message,
        uint8_t effect_id, TupParameterArgs *params, size_t n_params)
{
    int ret;
    int i, j;

    smp_message_init(message, TUP_MESSAGE_CMD_SET_PARAMETER);
    smp_message_set_uint8(message, 0, effect_id);

    for (i = 0, j = 1; i < n_params; i++, j += 2) {
        ret = smp_message_set_uint8(message, j, params[i].parameter_id);
        if (ret < 0)
            return ret;

        ret = smp_message_set_uint32(message, j + 1, params[i].parameter_value);
        if (ret < 0)
            return ret;
    }

    return 0;
}

int tup_message_parse_set_parameter(TupMessage *message,
        uint8_t *effect_id, TupParameterArgs *params, size_t size)
{
    int n_params = 0;
    int ret;
    int i, j;

    if (smp_message_get_msgid(message) != TUP_MESSAGE_CMD_SET_PARAMETER)
        return -EBADMSG;

    ret = smp_message_get_uint8(message, 0, effect_id);
    if (ret < 0)
        return ret;

    /* we use two args in SmpMessage for one parameter */
    n_params = (smp_message_n_args(message) - 1) / 2;
    if (size < n_params)
        return -ENOMEM;

    for (i = 0, j = 1; i < n_params; i++, j += 2) {
        ret = smp_message_get_uint8(message, j, &params[i].parameter_id);
        if (ret < 0)
            return ret;

        ret = smp_message_get_uint32(message, j + 1,
                &params[i].parameter_value);
        if (ret < 0)
            return ret;
    }

    return n_params;
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

int tup_message_init_get_sensor_value(TupMessage *message, int sensor_id, ...)
{
    va_list ap;
    int ret;

    va_start(ap, sensor_id);
    ret = tup_message_init_get_sensor_value_valist(message, sensor_id, ap);
    va_end(ap);

    return ret;
}

int tup_message_init_get_sensor_value_valist(TupMessage *message, int sensor_id,
        va_list varargs)
{
    int ret;
    int i;

    smp_message_init(message, TUP_MESSAGE_CMD_GET_SENSOR_VALUE);

    for (i = 0; sensor_id != -1; i++) {
        ret = smp_message_set_uint8(message, i, sensor_id);
        if (ret < 0)
            return ret;

        sensor_id = va_arg(varargs, int);
    }

    return 0;
}

void tup_message_init_get_sensor_value_simple(TupMessage *message,
        uint8_t sensor_id)
{
    tup_message_init_get_sensor_value(message, sensor_id, -1);
}

int tup_message_init_get_sensor_value_array(TupMessage *message,
        uint8_t *sensor_ids, size_t n_sensors)
{
    int ret;
    int i;

    smp_message_init(message, TUP_MESSAGE_CMD_GET_SENSOR_VALUE);
    for (i = 0; i < n_sensors; i++) {
        ret = smp_message_set_uint8(message, i, sensor_ids[i]);
        if (ret < 0)
            return ret;
    }

    return 0;
}

int tup_message_parse_get_sensor_value(TupMessage *message,
        uint8_t *sensor_ids, size_t size)
{
    int n_params = 0;
    int ret;
    int i;

    if (smp_message_get_msgid(message) != TUP_MESSAGE_CMD_GET_SENSOR_VALUE)
        return -EBADMSG;

    n_params = smp_message_n_args(message);
    if (size < n_params)
        return -ENOMEM;

    for (i = 0; i < n_params; i++) {
        ret = smp_message_get_uint8(message, i, &sensor_ids[i]);
        if (ret < 0) {
            /* should not happen */
            return ret;
        }
    }

    return n_params;
}

int tup_message_init_set_sensor_value(TupMessage *message, int sensor_id,
        int sensor_value, ...)
{
    va_list ap;
    int ret;

    va_start(ap, sensor_value);
    ret = tup_message_init_set_sensor_value_valist(message, sensor_id,
            sensor_value, ap);
    va_end(ap);

    return ret;
}

int tup_message_init_set_sensor_value_valist(TupMessage *message, int sensor_id,
        int sensor_value, va_list varargs)
{
    int ret;
    int i;

    smp_message_init(message, TUP_MESSAGE_CMD_SET_SENSOR_VALUE);

    for (i = 0; sensor_id != -1; i += 2) {
        ret = smp_message_set_uint8(message, i, sensor_id);
        if (ret < 0)
            return ret;

        ret = smp_message_set_uint16(message, i + 1, sensor_value);
        if (ret < 0)
            return ret;

        sensor_id = va_arg(varargs, int);
        sensor_value = va_arg(varargs, int);
    }

    return 0;
}

void tup_message_init_set_sensor_value_simple(TupMessage *message,
        int sensor_id, uint16_t sensor_value)
{
    tup_message_init_set_sensor_value(message, sensor_id, sensor_value, -1);
}

int tup_message_init_set_sensor_value_array(TupMessage *message,
        TupSensorValueArgs *args, size_t n_args)
{
    int ret;
    int i, j;

    smp_message_init(message, TUP_MESSAGE_CMD_SET_SENSOR_VALUE);
    for (i = 0, j = 0; i < n_args; i++, j += 2) {
        ret = smp_message_set_uint8(message, j, args[i].sensor_id);
        if (ret < 0)
            return ret;

        ret = smp_message_set_uint16(message, j + 1, args[i].sensor_value);
        if (ret < 0)
            return ret;
    }

    return 0;
}

int tup_message_parse_set_sensor_value(TupMessage *message,
        TupSensorValueArgs *args, size_t size)
{
    int n_params = 0;
    int ret;
    int i, j;

    if (smp_message_get_msgid(message) != TUP_MESSAGE_CMD_SET_SENSOR_VALUE)
        return -EBADMSG;

    /* we use two args in SmpMessage for one sensor value */
    n_params = smp_message_n_args(message) / 2;
    if (size < n_params)
        return -ENOMEM;

    for (i = 0, j = 0; i < n_params; i++, j += 2) {
        ret = smp_message_get_uint8(message, j, &args[i].sensor_id);
        if (ret < 0)
            return ret;

        ret = smp_message_get_uint16(message, j + 1, &args[i].sensor_value);
        if (ret < 0)
            return ret;
    }

    return n_params;
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

int tup_message_init_resp_parameter(TupMessage *message, uint8_t effect_id,
        TupParameterArgs *args, size_t n_args)
{
    int ret;
    int i, j;

    smp_message_init(message, TUP_MESSAGE_RESP_PARAMETER);
    smp_message_set_uint8(message, 0, effect_id);

    for (i = 0, j = 1; i < n_args; i++, j += 2) {
        ret = smp_message_set_uint8(message, j, args[i].parameter_id);
        if (ret < 0)
            return ret;

        ret = smp_message_set_uint32(message, j + 1, args[i].parameter_value);
        if (ret < 0)
            return ret;
    }

    return 0;
}

int tup_message_parse_resp_parameter(TupMessage *message, uint8_t *effect_id,
        TupParameterArgs *args, size_t size)
{
    int n_params = 0;
    int ret;
    int i, j;

    if (smp_message_get_msgid(message) != TUP_MESSAGE_RESP_PARAMETER)
        return -EBADMSG;

    ret = smp_message_get_uint8(message, 0, effect_id);
    if (ret < 0)
        return ret;

    /* we use two args in SmpMessage for one parameter */
    n_params = (smp_message_n_args(message) - 1) / 2;
    if (size < n_params)
        return -ENOMEM;

    for (i = 0, j = 1; i < n_params; i++, j += 2) {
        ret = smp_message_get_uint8(message, j, &args[i].parameter_id);
        if (ret < 0)
            return ret;

        ret = smp_message_get_uint32(message, j + 1, &args[i].parameter_value);
        if (ret < 0)
            return ret;
    }

    return n_params;
}

int tup_message_init_resp_sensor(TupMessage *message, TupSensorValueArgs *args,
        size_t n_args)
{
    int ret;
    int i, j;

    smp_message_init(message, TUP_MESSAGE_RESP_SENSOR);

    for (i = 0, j = 0; i < n_args; i++, j += 2) {
        ret = smp_message_set_uint8(message, j, args[i].sensor_id);
        if (ret < 0)
            return ret;

        ret = smp_message_set_uint16(message, j + 1, args[i].sensor_value);
        if (ret < 0)
            return ret;
    }

    return 0;
}

int tup_message_parse_resp_sensor(TupMessage *message, TupSensorValueArgs *args,
        size_t size)
{
    int n_params = 0;
    int ret;
    int i, j;

    if (smp_message_get_msgid(message) != TUP_MESSAGE_RESP_SENSOR)
        return -EBADMSG;

    /* we use two args in SmpMessage for one parameter */
    n_params = smp_message_n_args(message) / 2;
    if (size < n_params)
        return -ENOMEM;

    for (i = 0, j = 0; i < n_params; i++, j += 2) {
        ret = smp_message_get_uint8(message, j, &args[i].sensor_id);
        if (ret < 0)
            return ret;

        ret = smp_message_get_uint16(message, j + 1, &args[i].sensor_value);
        if (ret < 0)
            return ret;
    }

    return n_params;
}
