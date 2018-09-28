/* libtup
 * Copyright (C) 2017 Actronika SAS
 *     Author: Aurélien Zanelli <aurelien.zanelli@actronika.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * @file
 * \defgroup message Message
 *
 * TUP message definition
 */

#ifndef TUP_ENABLE_STATIC_API
#define TUP_ENABLE_STATIC_API
#endif

#include "libtup.h"
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

/**
 * \ingroup message
 * Create a new TupMessage. Message shall be inititialize afterward
 *
 * @return a new TupMessage or NULL on error.
 */
TupMessage *tup_message_new()
{
    return smp_message_new();
}

/**
 * \ingroup context
 * Create a new TupMessage object from a static storage.
 * @warning for now the TupMessage is only a typedef of SmpMessage so you can
 * pass NULL and 0 for smsg an struct_size repectively.
 *
 * @param[in] smsg a TupStaticMessage (not used)
 * @param[in] struct_size the size of smsg (not used)
 * @param[in] smp_msg a SmpMessage to use
 *
 * @return a SmpContext or NULL on error.
 */
TupMessage *tup_message_new_from_static(TupStaticMessage *smsg,
        size_t struct_size, SmpMessage *smp_msg)
{
    return smp_msg;
}

/**
 * \ingroup message
 * Free a previously allocated TupMessage.
 *
 * @param[in] message the TupMessage.
 */
void tup_message_free(TupMessage *message)
{
    return smp_message_free(message);
}

/**
 * \ingroup message
 * Clear a tup message.
 *
 * @param[in] message the TupMessage
 */
void tup_message_clear(TupMessage *message)
{
    smp_message_clear(message);
}

/**
 * \ingroup message
 * Get the type of a TupMessage.
 *
 * @param[in] message the TupMessage
 *
 * @return the message type as a TupMessageType.
 */
TupMessageType tup_message_get_type(TupMessage *message)
{
    return smp_message_get_msgid(message);
}

/**
 * \ingroup message
 * Initialize an ACK message for a given TupMessageType.
 *
 * @param[in] message the TupMessage
 * @param[in] cmd the TupMessageType of the acknowledged message
 */
void tup_message_init_ack(TupMessage *message, TupMessageType cmd)
{
    tup_message_init_ack_full(message, cmd, 0);
}

/**
 * \ingroup message
 * Initialize an ACK message for a given TupMessageType.
 *
 * @param[in] message the TupMessage
 * @param[in] cmd the TupMessageType of the acknowledged message
 * @param[in] arg1 an arbitrary uint32_t data
 */
void tup_message_init_ack_full(TupMessage *message, TupMessageType cmd,
        uint32_t arg1)
{
    smp_message_set_id(message, TUP_MESSAGE_ACK);
    smp_message_set(message, 0, SMP_TYPE_UINT32, cmd,
            1, SMP_TYPE_UINT32, arg1,
            -1);
}

/**
 * \ingroup message
 * Parse an ACK message and set the message which was acknowledged.
 *
 * @param[in] message the TupMessage
 * @param[out] cmd a pointer to a TupMessageType to hold the acknowledge message
 *                 type
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_parse_ack(TupMessage *message, TupMessageType *cmd)
{
    return tup_message_parse_ack_full(message, cmd, NULL);
}

/**
 * \ingroup message
 * Parse an ACK message and set the message which was acknowledged.
 *
 * @param[in] message the TupMessage
 * @param[out] cmd a pointer to a TupMessageType to hold the acknowledge message
 *                 type
 * @param[out] arg1 an arbitrary uint32_t data (can be NULL)
 *
 * @return 0 on success, a negative errno value otherwise.
 */
int tup_message_parse_ack_full(TupMessage *message, TupMessageType *cmd,
        uint32_t *arg1)
{
    uint32_t cmd_id, arg1_tmp;
    int ret;

    if (smp_message_get_msgid(message) != TUP_MESSAGE_ACK)
        return SMP_ERROR_BAD_MESSAGE;

    ret = smp_message_get(message, 0, SMP_TYPE_UINT32, &cmd_id,
            1, SMP_TYPE_UINT32, &arg1_tmp,
            -1);
    if (ret < 0)
        return ret;

    *cmd = (TupMessageType) cmd_id;

    if (arg1 != NULL)
        *arg1 = arg1_tmp;

    return 0;
}

/**
 * \ingroup message
 * Initialize an error message with given message type and error.
 *
 * @param[in] message the TupMessage
 * @param[in] cmd the TupMessageType error is for
 * @param[in] error the error code
 */
void tup_message_init_error(TupMessage *message, TupMessageType cmd,
        uint32_t error)
{
    tup_message_init_error_full(message, TUP_MESSAGE_ERROR, error, 0);
}

/**
 * \ingroup message
 * Initialize an error message with given message type and error.
 *
 * @param[in] message the TupMessage
 * @param[in] cmd the TupMessageType error is for
 * @param[in] error the error code
 * @param[in] arg1 an arbitrary uint32_t data
 */
void tup_message_init_error_full(TupMessage *message, TupMessageType cmd,
        uint32_t error, uint32_t arg1)
{
    smp_message_set_id(message, TUP_MESSAGE_ERROR);
    smp_message_set(message, 0, SMP_TYPE_UINT32, cmd,
            1, SMP_TYPE_UINT32, error,
            2, SMP_TYPE_UINT32, arg1,
            -1);
}

/**
 * \ingroup message
 * Parse an error message.
 *
 * @param[in] message the TupMessage
 * @param[out] cmd the TupMessageType error is for
 * @param[out] error the error code
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_parse_error(TupMessage *message, TupMessageType *cmd,
        uint32_t *error)
{
    return tup_message_parse_error_full(message, cmd, error, NULL);
}

/**
 * \ingroup message
 * Parse an error message.
 *
 * @param[in] message the TupMessage
 * @param[out] cmd the TupMessageType error is for
 * @param[out] error the error code
 * @param[out] arg1 an arbitrary uint32_t data (can be NULL)
 *
 * @return 0 on success, a negative errno value otherwise.
 */
int tup_message_parse_error_full(TupMessage *message, TupMessageType *cmd,
        uint32_t *error, uint32_t *arg1)
{
    uint32_t cmd_id, arg1_tmp;
    int ret;

    if (smp_message_get_msgid(message) != TUP_MESSAGE_ERROR)
        return SMP_ERROR_BAD_MESSAGE;

    ret = smp_message_get(message, 0, SMP_TYPE_UINT32, &cmd_id,
            1, SMP_TYPE_UINT32, error,
            2, SMP_TYPE_UINT32, &arg1_tmp,
            -1);
    if (ret < 0)
        return ret;

    if (arg1 != NULL)
        *arg1 = arg1_tmp;

    *cmd = cmd_id;
    return 0;
}

/**
 * \ingroup message
 * Initialize a load message.
 *
 * @param[in] message the TupMessage
 * @param[in] effect_id the slot in which the effect should be loaded
 * @param[in] bank_id the id of the effect in the library
 */
void tup_message_init_load(TupMessage *message, uint8_t effect_id,
        uint16_t bank_id)
{
    smp_message_set_id(message, TUP_MESSAGE_CMD_LOAD);
    smp_message_set_uint8(message, 0, effect_id);
    smp_message_set_uint16(message, 1, bank_id);
}

/**
 * \ingroup message
 * Parse a load message
 *
 * @param[in] message the TupMessage
 * @param[out] effect_id the slot in which the effect should be loaded
 * @param[out] bank_id the id of the effect in the library
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_parse_load(TupMessage *message, uint8_t *effect_id,
        uint16_t *bank_id)
{
    if (smp_message_get_msgid(message) != TUP_MESSAGE_CMD_LOAD)
        return SMP_ERROR_BAD_MESSAGE;

    return smp_message_get(message, 0, SMP_TYPE_UINT8, effect_id,
            1, SMP_TYPE_UINT16, bank_id, -1);
}

/**
 * \ingroup message
 * Initialize a play message.
 *
 * @param[in] message the TupMessage
 * @param[in] effect_id the loaded effect to play
 */
void tup_message_init_play(TupMessage *message, uint8_t effect_id)
{
    smp_message_set_id(message, TUP_MESSAGE_CMD_PLAY);
    smp_message_set_uint8(message, 0, effect_id);
}

/**
 * \ingroup message
 * Parse a play message.
 *
 * @param[in] message the TupMessage
 * @param[out] effect_id the loaded effect to play
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_parse_play(TupMessage *message, uint8_t *effect_id)
{
    if (smp_message_get_msgid(message) != TUP_MESSAGE_CMD_PLAY)
        return SMP_ERROR_BAD_MESSAGE;

    return smp_message_get_uint8(message, 0, effect_id);
}

/**
 * \ingroup message
 * Initialize a stop message.
 *
 * @param[in] message the TupMessage
 * @param[in] effect_id the loaded effect to stop
 */
void tup_message_init_stop(TupMessage *message, uint8_t effect_id)
{
    smp_message_set_id(message, TUP_MESSAGE_CMD_STOP);
    smp_message_set_uint8(message, 0, effect_id);
}

/**
 * \ingroup message
 * Parse a stop message.
 *
 * @param[in] message the TupMessage
 * @param[out] effect_id the loaded effect to stop
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_parse_stop(TupMessage *message, uint8_t *effect_id)
{
    if (smp_message_get_msgid(message) != TUP_MESSAGE_CMD_STOP)
        return SMP_ERROR_BAD_MESSAGE;

    return smp_message_get_uint8(message, 0, effect_id);
}

/**
 * \ingroup message
 * Initialize a get_version message.
 *
 * @param[in] message the TupMessage
 */
void tup_message_init_get_version(TupMessage *message)
{
    smp_message_set_id(message, TUP_MESSAGE_CMD_GET_VERSION);
}

/**
 * \ingroup message
 * Initialize a get_parameter message.
 * Variable argument is a list of parameter_id. Last parameter_id shall be -1.
 *
 * @param[in] message the TupMessage
 * @param[in] effect_id the loaded effect id
 * @param[in] parameter_id id of the first parameter to get
 * @param[in] ... variable argument
 *
 * @return 0 on success, a SmpError otherwise.
 */
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

/**
 * \ingroup message
 * Initialize a get_parameter message with a va_list.
 * Variable argument is a list of parameter_id. Last parameter_id shall be -1.
 *
 * @param[in] message the TupMessage
 * @param[in] effect_id the loaded effect id
 * @param[in] parameter_id id of the first parameter to get
 * @param[in] varargs a va_list of parameter_id to get
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_init_get_parameter_valist(TupMessage *message,
        uint8_t effect_id, int parameter_id, va_list varargs)
{
    int ret;
    int i;

    smp_message_set_id(message, TUP_MESSAGE_CMD_GET_PARAMETER);
    smp_message_set_uint8(message, 0, effect_id);

    for (i = 1; parameter_id != -1; i++) {
        ret = smp_message_set_uint8(message, i, parameter_id);
        if (ret < 0)
            return ret;

        parameter_id = va_arg(varargs, int);
    }

    return 0;
}

/**
 * \ingroup message
 * Initialize a get_parameter message with one parameter.
 *
 * @param[in] message the TupMessage
 * @param[in] effect_id the loaded effect id
 * @param[in] parameter_id id of the parameter to get
 */
void tup_message_init_get_parameter_simple(TupMessage *message,
        uint8_t effect_id, uint8_t parameter_id)
{
    tup_message_init_get_parameter(message, effect_id, parameter_id, -1);
}

/**
 * \ingroup message
 * Initialize a get_parameter message from an array of ids.
 *
 * @param[in] message the TupMessage
 * @param[in] effect_id the loaded effect id
 * @param[in] parameter_ids an array of parameter ids to get
 * @param[in] n_parameters the number of parameters in parameter_ids
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_init_get_parameter_array(TupMessage *message,
        uint8_t effect_id, uint8_t *parameter_ids, size_t n_parameters)
{
    int ret;
    size_t i;

    smp_message_set_id(message, TUP_MESSAGE_CMD_GET_PARAMETER);
    smp_message_set_uint8(message, 0, effect_id);

    for (i = 0; i < n_parameters; i++) {
        ret = smp_message_set_uint8(message, i + 1, parameter_ids[i]);
        if (ret < 0)
            return ret;
    }

    return 0;
}

/**
 * \ingroup message
 * Initialize a get_parameter with only effect_id. User must use
 * tup_message_init_get_parameter_set_parameter_id() afterward to set the
 * requeated ids.
 *
 * @param[in] message the TupMessage
 * @param[in] effect_id the loaded effect id
 */
void tup_message_init_get_parameter_set_effect_id(TupMessage *message,
        uint8_t effect_id)
{
    smp_message_set_id(message, TUP_MESSAGE_CMD_GET_PARAMETER);
    smp_message_set_uint8(message, 0, effect_id);
}

/**
 * \ingroup message
 * Set a parameter id to request in a get_parameter message. User must have
 * called tup_message_init_get_parameter_set_effect_id() before calling this
 * method.
 *
 * @param[in] message the TupMessage
 * @param[in] index the Nth parameter to request
 * @param[in] parameter_id the parameter id to request
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_init_get_parameter_set_parameter_id(TupMessage *message,
        unsigned int index, uint8_t parameter_id)
{
    return smp_message_set_uint8(message, 1 + index, parameter_id);
}

/**
 * \ingroup message
 * Parse a get_parameter message and store the result in effect_id and
 * parameters_ids.
 *
 * @param[in] message the TupMessage
 * @param[out] effect_id the load effect id from which parameter should be get
 * @param[out] parameter_ids pointer to an array of parameter ids
 * @param[in] size the size of parameter_ids array
 *
 * @return the number of parameters to get on success, a SmpError otherwise.
 */
int tup_message_parse_get_parameter(TupMessage *message, uint8_t *effect_id,
        uint8_t *parameter_ids, size_t size)
{
    int n_params = 0;
    int ret;
    int i;

    if (smp_message_get_msgid(message) != TUP_MESSAGE_CMD_GET_PARAMETER)
        return SMP_ERROR_BAD_MESSAGE;

    ret = smp_message_get_uint8(message, 0, effect_id);
    if (ret < 0)
        return ret;

    /* n_params >= 0 as we already get one arg */
    n_params = smp_message_n_args(message) - 1;
    if (size < (size_t) n_params) {
        /* array too small */
        return SMP_ERROR_OVERFLOW;
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

/**
 * \ingroup message
 * Initialize a set_parameter message.
 * Variable argument is a list of parameter id and parameter value. The last
 * parameter id shall be -1.
 *
 * @param[in] message the TupMessage
 * @param[in] effect_id the loaded effect id
 * @param[in] parameter_id id of the first parameter to set
 * @param[in] parameter_value value of the first parameter to set
 * @param[in] ... variable argument
 *
 * @return 0 on success, a SmpError otherwise.
 */
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

/**
 * \ingroup message
 * Initialize a set_parameter message with a va_list.
 * Variable argument is a list of parameter id and parameter value. The last
 * parameter id shall be -1.
 *
 * @param[in] message the TupMessage
 * @param[in] effect_id the loaded effect id
 * @param[in] parameter_id id of the first parameter to set
 * @param[in] parameter_value value of the first parameter to set
 * @param[in] varargs variable argument
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_init_set_parameter_valist(TupMessage *message,
        uint8_t effect_id, int parameter_id, uint32_t parameter_value,
        va_list varargs)
{
    int ret;
    int i;

    smp_message_set_id(message, TUP_MESSAGE_CMD_SET_PARAMETER);
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

/**
 * \ingroup message
 * Initialize a set_parameter message with one parameter.
 *
 * @param[in] message the TupMessage
 * @param[in] effect_id the loaded effect id
 * @param[in] parameter_id id of the parameter to set
 * @param[in] parameter_value the value to set
 */
void tup_message_init_set_parameter_simple(TupMessage *message,
        uint8_t effect_id, uint8_t parameter_id, uint32_t parameter_value)
{
    tup_message_init_set_parameter(message, effect_id, parameter_id,
            parameter_value, -1);
}

/**
 * \ingroup message
 * Initialize a set_parameter message from an array of TupParameterArgs.
 *
 * @param[in] message the TupMessage
 * @param[in] effect_id the loaded effect id
 * @param[in] params an array of TupParameterArgs
 * @param[in] n_params the number of parameters in TupParameterArgs
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_init_set_parameter_array(TupMessage *message,
        uint8_t effect_id, TupParameterArgs *params, size_t n_params)
{
    int ret;
    size_t i, j;

    smp_message_set_id(message, TUP_MESSAGE_CMD_SET_PARAMETER);
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

/**
 * \ingroup message
 * Parse a set_parameter message and store the result in effect_id and
 * params.
 *
 * @param[in] message the TupMessage
 * @param[out] effect_id the load effect id from which parameter should be set
 * @param[out] params pointer to an array of TupParameterArgs
 * @param[in] size the size of params array
 *
 * @return the number of parameters to set on success, a SmpError otherwise.
 */
int tup_message_parse_set_parameter(TupMessage *message,
        uint8_t *effect_id, TupParameterArgs *params, size_t size)
{
    int n_params = 0;
    int ret;
    int i, j;

    if (smp_message_get_msgid(message) != TUP_MESSAGE_CMD_SET_PARAMETER)
        return SMP_ERROR_BAD_MESSAGE;

    ret = smp_message_get_uint8(message, 0, effect_id);
    if (ret < 0)
        return ret;

    /* we use two args in SmpMessage for one parameter and n_args > 0 as we
     * already get one */
    n_params = (smp_message_n_args(message) - 1) / 2;
    if (size < (size_t) n_params)
        return SMP_ERROR_OVERFLOW;

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

/**
 * \ingroup message
 * Initialize a bind effect message.
 *
 * @param[in] message the TupMessage
 * @param[in] effect_id the loaded effect id to bind to
 * @param[in] binding_flags the binding flags
 */
void tup_message_init_bind_effect(TupMessage *message, uint8_t effect_id,
        unsigned int binding_flags)
{
    smp_message_set_id(message, TUP_MESSAGE_CMD_BIND_EFFECT);
    smp_message_set_uint8(message, 0, effect_id);
    smp_message_set_uint8(message, 1, binding_flags);
}

/**
 * \ingroup message
 * Parse a bind effect message.
 *
 * @param[in] message the TupMessage
 * @param[out] effect_id the loaded effect id to bind to
 * @param[out] binding_flags the binding flags
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_parse_bind_effect(TupMessage *message, uint8_t *effect_id,
        unsigned int *binding_flags)
{
    uint8_t flags = 0;
    int ret;

    if (smp_message_get_msgid(message) != TUP_MESSAGE_CMD_BIND_EFFECT)
        return SMP_ERROR_BAD_MESSAGE;

    ret = smp_message_get(message, 0, SMP_TYPE_UINT8, effect_id,
            1, SMP_TYPE_UINT8, &flags, -1);

    *binding_flags = flags;
    return ret;
}

/**
 * \ingroup message
 * Initialize a get_sensor_value message.
 * Variable argument is a list of sensor id. Last sensor id shall be -1.
 *
 * @param[in] message the TupMessage
 * @param[in] sensor_id id of the first sensor value to get
 * @param[in] ... variable argument
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_init_get_sensor_value(TupMessage *message, int sensor_id, ...)
{
    va_list ap;
    int ret;

    va_start(ap, sensor_id);
    ret = tup_message_init_get_sensor_value_valist(message, sensor_id, ap);
    va_end(ap);

    return ret;
}

/**
 * \ingroup message
 * Initialize a get_sensor_value message using a valist.
 * Variable argument is a list of sensor id. Last sensor id shall be -1.
 *
 * @param[in] message the TupMessage
 * @param[in] sensor_id id of the first sensor value to get
 * @param[in] varargs variable argument
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_init_get_sensor_value_valist(TupMessage *message, int sensor_id,
        va_list varargs)
{
    int ret;
    int i;

    smp_message_set_id(message, TUP_MESSAGE_CMD_GET_SENSOR_VALUE);

    for (i = 0; sensor_id != -1; i++) {
        ret = smp_message_set_uint8(message, i, sensor_id);
        if (ret < 0)
            return ret;

        sensor_id = va_arg(varargs, int);
    }

    return 0;
}

/**
 * \ingroup message
 * Initialize a get_sensor_value message with one sensor.
 *
 * @param[in] message the TupMessage
 * @param[in] sensor_id id of the sensor value to get
 */
void tup_message_init_get_sensor_value_simple(TupMessage *message,
        uint8_t sensor_id)
{
    tup_message_init_get_sensor_value(message, sensor_id, -1);
}

/**
 * \ingroup message
 * Initialize a get_sensor_value message from an array of ids.
 *
 * @param[in] message the TupMessage
 * @param[in] sensor_ids an array of sensor ids to get
 * @param[in] n_sensors the number of sensor ids in sensor_ids
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_init_get_sensor_value_array(TupMessage *message,
        uint8_t *sensor_ids, size_t n_sensors)
{
    int ret;
    size_t i;

    smp_message_set_id(message, TUP_MESSAGE_CMD_GET_SENSOR_VALUE);
    for (i = 0; i < n_sensors; i++) {
        ret = smp_message_set_uint8(message, i, sensor_ids[i]);
        if (ret < 0)
            return ret;
    }

    return 0;
}

/**
 * \ingroup message
 * Parse a get_sensor_value message and store the result in sensor_ids.
 *
 * @param[in] message the TupMessage
 * @param[out] sensor_ids pointer to an array of sensor ids
 * @param[in] size the size of sensor_ids array
 *
 * @return the number of sensor to get on success, a SmpError otherwise.
 */
int tup_message_parse_get_sensor_value(TupMessage *message,
        uint8_t *sensor_ids, size_t size)
{
    int n_params = 0;
    int ret;
    int i;

    if (smp_message_get_msgid(message) != TUP_MESSAGE_CMD_GET_SENSOR_VALUE)
        return SMP_ERROR_BAD_MESSAGE;

    /* n_params >= 0 as we already get one arg */
    n_params = smp_message_n_args(message);
    if (size < (size_t) n_params)
        return SMP_ERROR_OVERFLOW;

    for (i = 0; i < n_params; i++) {
        ret = smp_message_get_uint8(message, i, &sensor_ids[i]);
        if (ret < 0) {
            /* should not happen */
            return ret;
        }
    }

    return n_params;
}

/**
 * \ingroup message
 * Initialize a set_sensor_value message.
 * Variable argument is a list of sensor id and sensor value.
 * Last sensor id shall be -1.
 *
 * @param[in] message the TupMessage
 * @param[in] sensor_id id of the first sensor value to set
 * @param[in] sensor_value the first value to set
 * @param[in] ... variable argument
 *
 * @return 0 on success, a SmpError otherwise.
 */
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

/**
 * \ingroup message
 * Initialize a set_sensor_value message using va_list.
 * Variable argument is a list of sensor id and sensor value.
 * Last sensor id shall be -1.
 *
 * @param[in] message the TupMessage
 * @param[in] sensor_id id of the first sensor value to set
 * @param[in] sensor_value the first value to set
 * @param[in] varargs variable argument
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_init_set_sensor_value_valist(TupMessage *message, int sensor_id,
        int sensor_value, va_list varargs)
{
    int ret;
    int i;

    smp_message_set_id(message, TUP_MESSAGE_CMD_SET_SENSOR_VALUE);

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

/**
 * \ingroup message
 * Initialize a set_sensor_value message for one sensor.
 *
 * @param[in] message the TupMessage
 * @param[in] sensor_id id of the sensor value to set
 * @param[in] sensor_value the value to set
 */
void tup_message_init_set_sensor_value_simple(TupMessage *message,
        int sensor_id, uint16_t sensor_value)
{
    tup_message_init_set_sensor_value(message, sensor_id, sensor_value, -1);
}

/**
 * \ingroup message
 * Initialize a set_sensor_value message from an array of TupSensorValueArgs.
 *
 * @param[in] message the TupMessage
 * @param[in] args an array of TupSensorArgs
 * @param[in] n_args the number of args in TupSensorValueArgs
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_init_set_sensor_value_array(TupMessage *message,
        TupSensorValueArgs *args, size_t n_args)
{
    int ret;
    size_t i, j;

    smp_message_set_id(message, TUP_MESSAGE_CMD_SET_SENSOR_VALUE);
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

/**
 * \ingroup message
 * Parse a set_sensor_value message and store the result in args.
 *
 * @param[in] message the TupMessage
 * @param[out] args pointer to an array of TupSensorValueArgs
 * @param[in] size the size of args array
 *
 * @return the number of sensor value to set on success, a SmpError otherwise.
 */
int tup_message_parse_set_sensor_value(TupMessage *message,
        TupSensorValueArgs *args, size_t size)
{
    int n_params = 0;
    int ret;
    int i, j;

    if (smp_message_get_msgid(message) != TUP_MESSAGE_CMD_SET_SENSOR_VALUE)
        return SMP_ERROR_BAD_MESSAGE;

    /* we use two args in SmpMessage for one sensor value */
    n_params = smp_message_n_args(message) / 2;
    if (size < (size_t) n_params)
        return SMP_ERROR_OVERFLOW;

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

/**
 * \ingroup message
 * Initialize a get_input_value message.
 * Variable argument is a list of input id. Last input id shall be -1.
 *
 * @param[in] message the TupMessage
 * @param[in] effect_slot_id id of the effect to update values
 * @param[in] input_id id of the first input value to get
 * @param[in] ... variable argument
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_init_get_input_value(TupMessage *message, int effect_slot_id,
        int input_id, ...)
{
    va_list ap;
    int ret;

    va_start(ap, input_id);
    ret = tup_message_init_get_input_value_valist(message, effect_slot_id,
            input_id, ap);
    va_end(ap);

    return ret;
}

/**
 * \ingroup message
 * Initialize a get_input_value message using a valist.
 * Variable argument is a list of input id. Last input id shall be -1.
 *
 * @param[in] message the TupMessage
 * @param[in] effect_slot_id id of the effect to update values
 * @param[in] input_id id of the first input value to get
 * @param[in] varargs variable argument
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_init_get_input_value_valist(TupMessage *message,
        int effect_slot_id, int input_id, va_list varargs)
{
    int ret;
    int i;

    smp_message_set_id(message, TUP_MESSAGE_CMD_GET_INPUT_VALUE);

    ret = smp_message_set_uint8(message, 0, effect_slot_id);
    if (ret < 0)
        return ret;

    for (i = 1; input_id != -1; i++) {
        ret = smp_message_set_uint8(message, i, input_id);
        if (ret < 0)
            return ret;

        input_id = va_arg(varargs, int);
    }

    return 0;
}

/**
 * \ingroup message
 * Initialize a get_input_value message with one input.
 *
 * @param[in] message the TupMessage
 * @param[in] effect_slot_id id of the effect to update values
 * @param[in] input_id id of the input value to get
 */
void tup_message_init_get_input_value_simple(TupMessage *message,
        uint8_t effect_slot_id, uint8_t input_id)
{
    tup_message_init_get_input_value(message, effect_slot_id, input_id, -1);
}

/**
 * \ingroup message
 * Initialize a get_input_value message from an array of ids.
 *
 * @param[in] message the TupMessage
 * @param[in] effect_slot_id id of the effect to update values
 * @param[in] input_ids an array of input ids to get
 * @param[in] n_inputs the number of input ids in input_ids
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_init_get_input_value_array(TupMessage *message,
        uint8_t effect_slot_id, uint8_t *input_ids, size_t n_inputs)
{
    int ret;
    size_t i;

    smp_message_set_id(message, TUP_MESSAGE_CMD_GET_INPUT_VALUE);

    ret = smp_message_set_uint8(message, 0, effect_slot_id);
    if (ret < 0)
        return ret;

    for (i = 1; i <= n_inputs; i++) {
        ret = smp_message_set_uint8(message, i, input_ids[i]);
        if (ret < 0)
            return ret;
    }

    return 0;
}

/**
 * \ingroup message
 * Parse a get_input_value message and store the result in input_ids.
 *
 * @param[in] message the TupMessage
 * @param[out] effect_slot_id id of the effect to update values
 * @param[out] input_ids pointer to an array of input ids
 * @param[in] size the size of input_ids array
 *
 * @return the number of input to get on success, a SmpError otherwise.
 */
int tup_message_parse_get_input_value(TupMessage *message,
        uint8_t *effect_slot_id, uint8_t *input_ids, size_t size)
{
    int n_params = 0;
    int ret;
    int i;

    if (smp_message_get_msgid(message) != TUP_MESSAGE_CMD_GET_INPUT_VALUE)
        return SMP_ERROR_BAD_MESSAGE;

    /* n_params >= 0 as we already get one arg */
    n_params = smp_message_n_args(message) - 1;
    if (size < (size_t) n_params)
        return SMP_ERROR_OVERFLOW;

    ret = smp_message_get_uint8(message, 0, effect_slot_id);
    if (ret < 0) {
        /* should not happen */
        return ret;
    }

    for (i = 0; i < n_params; i++) {
        ret = smp_message_get_uint8(message, i+1, &input_ids[i]);
        if (ret < 0) {
            /* should not happen */
            return ret;
        }
    }

    return n_params;
}

/**
 * \ingroup message
 * Initialize a set_input_value message.
 * Variable argument is a list of input id and input value.
 * Last input id shall be -1.
 *
 * @param[in] message the TupMessage
 * @param[in] effect_slot_id id of the effect to update values
 * @param[in] input_id id of the first input value to set
 * @param[in] input_value the first value to set
 * @param[in] ... variable argument
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_init_set_input_value(TupMessage *message,
        int effect_slot_id, int input_id, int input_value, ...)
{
    va_list ap;
    int ret;

    va_start(ap, input_value);
    ret = tup_message_init_set_input_value_valist(message, effect_slot_id,
            input_id, input_value, ap);
    va_end(ap);

    return ret;
}

/**
 * \ingroup message
 * Initialize a set_input_value message using va_list.
 * Variable argument is a list of input id and input value.
 * Last input id shall be -1.
 *
 * @param[in] message the TupMessage
 * @param[in] effect_slot_id id of the effect to update values
 * @param[in] input_id id of the first input value to set
 * @param[in] input_value the first value to set
 * @param[in] varargs variable argument
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_init_set_input_value_valist(TupMessage *message,
        int effect_slot_id, int input_id, int input_value, va_list varargs)
{
    int ret;
    int i;

    smp_message_set_id(message, TUP_MESSAGE_CMD_SET_INPUT_VALUE);

    ret = smp_message_set_uint8(message, 0, effect_slot_id);
    if (ret < 0)
        return ret;

    for (i = 1; input_id != -1; i += 2) {
        ret = smp_message_set_uint8(message, i, input_id);
        if (ret < 0)
            return ret;

        ret = smp_message_set_int32(message, i + 1, input_value);
        if (ret < 0)
            return ret;

        input_id = va_arg(varargs, int);
        input_value = va_arg(varargs, int);
    }

    return 0;
}

/**
 * \ingroup message
 * Initialize a set_input_value message for one input.
 *
 * @param[in] message the TupMessage
 * @param[in] effect_slot_id id of the effect to update values
 * @param[in] input_id id of the input value to set
 * @param[in] input_value the value to set
 */
void tup_message_init_set_input_value_simple(TupMessage *message,
        uint8_t effect_slot_id, uint8_t input_id, int input_value)
{
    tup_message_init_set_input_value(message, effect_slot_id, input_id,
            input_value, -1);
}

/**
 * \ingroup message
 * Initialize a set_input_value message from an array of TupInputValueArgs.
 *
 * @param[in] message the TupMessage
 * @param[in] effect_slot_id id of the effect to update values
 * @param[in] args an array of TupInputArgs
 * @param[in] n_args the number of args in TupInputValueArgs
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_init_set_input_value_array(TupMessage *message,
        uint8_t effect_slot_id, TupInputValueArgs *args, size_t n_args)
{
    int ret;
    size_t i, j;

    smp_message_set_id(message, TUP_MESSAGE_CMD_SET_INPUT_VALUE);

    ret = smp_message_set_uint8(message, 0, effect_slot_id);
    if (ret < 0)
        return ret;

    for (i = 0, j = 1; i < n_args; i++, j += 2) {
        ret = smp_message_set_uint8(message, j, args[i].input_id);
        if (ret < 0)
            return ret;

        ret = smp_message_set_int32(message, j + 1, args[i].input_value);
        if (ret < 0)
            return ret;
    }

    return 0;
}

/**
 * \ingroup message
 * Parse a set_input_value message and store the result in args.
 *
 * @param[in] message the TupMessage
 * @param[out] effect_slot_id id of the effect to update values
 * @param[out] args pointer to an array of TupInputValueArgs
 * @param[in] size the size of args array
 *
 * @return the number of input value to set on success, a SmpError otherwise.
 */
int tup_message_parse_set_input_value(TupMessage *message,
        uint8_t *effect_slot_id, TupInputValueArgs *args, size_t size)
{
    int n_params = 0;
    int ret;
    int i, j;

    if (smp_message_get_msgid(message) != TUP_MESSAGE_CMD_SET_INPUT_VALUE)
        return SMP_ERROR_BAD_MESSAGE;

    /* we use two args in SmpMessage for one input value */
    n_params = (smp_message_n_args(message) - 1 ) / 2;
    if (size < (size_t) n_params)
        return SMP_ERROR_OVERFLOW;

    ret = smp_message_get_uint8(message, 0, effect_slot_id);
    if (ret < 0)
        return ret;

    for (i = 0, j = 1; i < n_params; i++, j += 2) {
        ret = smp_message_get_uint8(message, j, &args[i].input_id);
        if (ret < 0)
            return ret;

        ret = smp_message_get_int32(message, j + 1, &args[i].input_value);
        if (ret < 0)
            return ret;
    }

    return n_params;
}

/**
 * \ingroup message
 * Initialize an activation internal sensors message.
 *
 * @param[in] message the TupMessage
 * @param[in] state boolean to activate or not the internal sensors
 */
void tup_message_init_activate_internal_sensors(TupMessage *message,
        uint8_t state)
{
    smp_message_set_id(message, TUP_MESSAGE_CMD_ACTIVATE_INTERNAL_SENSORS);
    smp_message_set_uint8(message, 0, state);
}

/**
 * \ingroup message
 * Parse an activation message.
 *
 * @param[in] message the TupMessage
 * @param[in] state boolean to activate or not the internal sensors
 * @return 0 on success, a negative errno otherwise
 */
int tup_message_parse_activate_internal_sensors(TupMessage *message,
        uint8_t *state)
{
    int msgid = smp_message_get_msgid(message);

    if (msgid != TUP_MESSAGE_CMD_ACTIVATE_INTERNAL_SENSORS)
        return SMP_ERROR_BAD_MESSAGE;

    return smp_message_get_uint8(message, 0, state);
}

/**
 * \ingroup message
 * Initialize a get_buildinfo message.
 *
 * @param[in] message the TupMessage
 */
void tup_message_init_get_buildinfo(TupMessage *message)
{
    smp_message_set_id(message, TUP_MESSAGE_CMD_GET_BUILDINFO);
}

/**
 * \ingroup message
 * Initialize a version response message with the given version.
 * Warning: due to smp limitation about string, version should exist as long as
 * message exist.
 *
 * @param[in] message the TupMessage
 * @param[in] version the version to report
 */
void tup_message_init_resp_version(TupMessage *message, const char *version)
{
    smp_message_set_id(message, TUP_MESSAGE_RESP_VERSION);
    smp_message_set_cstring(message, 0, version);
}

/**
 * \ingroup message
 * Parse a version response message
 * Warning: version is not copied and so it is valid as long as message is
 * valid.
 *
 * @param[in] message the TupMessage
 * @param[out] version the version
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_parse_resp_version(TupMessage *message, const char **version)
{
    if (smp_message_get_msgid(message) != TUP_MESSAGE_RESP_VERSION)
        return SMP_ERROR_BAD_MESSAGE;

    return smp_message_get_cstring(message, 0, version);
}

/**
 * \ingroup message
 * Initialize a get_parameter response message.
 *
 * @param[in] message the TupMessage
 * @param[in] effect_id the loaded effect id
 * @param[in] args an array of TupParameterArgs
 * @param[in] n_args the number of parameters in TupParameterArgs
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_init_resp_parameter(TupMessage *message, uint8_t effect_id,
        TupParameterArgs *args, size_t n_args)
{
    int ret;
    size_t i, j;

    smp_message_set_id(message, TUP_MESSAGE_RESP_PARAMETER);
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

/**
 * \ingroup message
 * Parse a get_parameter response message.
 *
 * @param[in] message the TupMessage
 * @param[out] effect_id pointer to hold the loaded effect id
 * @param[out] args an array of TupParameterArgs
 * @param[in] size the size of args array
 *
 * @return the number of parameters on success, a SmpError otherwise.
 */
int tup_message_parse_resp_parameter(TupMessage *message, uint8_t *effect_id,
        TupParameterArgs *args, size_t size)
{
    int n_params = 0;
    int ret;
    int i, j;

    if (smp_message_get_msgid(message) != TUP_MESSAGE_RESP_PARAMETER)
        return SMP_ERROR_BAD_MESSAGE;

    ret = smp_message_get_uint8(message, 0, effect_id);
    if (ret < 0)
        return ret;

    /* we use two args in SmpMessage for one parameter and n_args > 0 as we
     * already get one */
    n_params = (smp_message_n_args(message) - 1) / 2;
    if (size < (size_t) n_params)
        return SMP_ERROR_OVERFLOW;

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

/**
 * \ingroup message
 * Get the effect id from a get_parameter response message.
 *
 * @param[in] message the TupMessage
 * @param[out] effect_id pointer to hold the loaded effect id
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_parse_resp_parameter_get_effect_id(TupMessage *message,
        uint8_t *effect_id)
{
    if (smp_message_get_msgid(message) != TUP_MESSAGE_RESP_PARAMETER)
        return SMP_ERROR_BAD_MESSAGE;

    return smp_message_get_uint8(message, 0, effect_id);
}

/**
 * \ingroup message
 * Get the Nth parameter of a get_parameter response message.
 *
 * @param[in] message the TupMessage
 * @param[in] index the Nth parameter to get
 * @param[out] arg the parameter.
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_parse_resp_parameter_get_parameter(TupMessage *message,
        unsigned int index, TupParameterArgs *arg)
{
    int ret;

    if (smp_message_get_msgid(message) != TUP_MESSAGE_RESP_PARAMETER)
        return SMP_ERROR_BAD_MESSAGE;

    ret = smp_message_get_uint8(message, 1 + 2 * index, &arg->parameter_id);
    if (ret < 0)
        return ret;

    ret = smp_message_get_uint32(message, 2 + 2 * index, &arg->parameter_value);
    if (ret < 0)
        return ret;

    return 0;
}

/**
 * \ingroup message
 * Initialize a get_sensor_value response message.
 *
 * @param[in] message the TupMessage
 * @param[in] args an array of TupSensorValueArgs
 * @param[in] n_args the number of elements in args
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_init_resp_sensor(TupMessage *message, TupSensorValueArgs *args,
        size_t n_args)
{
    int ret;
    size_t i, j;

    smp_message_set_id(message, TUP_MESSAGE_RESP_SENSOR);

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

/**
 * \ingroup message
 * Parse a get_sensor_value response message.
 *
 * @param[in] message the TupMessage
 * @param[out] args an array of TupSensorValueArgs
 * @param[in] size the size of args array
 *
 * @return the number of sensor value on success, a SmpError otherwise.
 */
int tup_message_parse_resp_sensor(TupMessage *message, TupSensorValueArgs *args,
        size_t size)
{
    int n_params = 0;
    int ret;
    int i, j;

    if (smp_message_get_msgid(message) != TUP_MESSAGE_RESP_SENSOR)
        return SMP_ERROR_BAD_MESSAGE;

    /* we use two args in SmpMessage for one parameter */
    n_params = smp_message_n_args(message) / 2;
    if (size < (size_t) n_params)
        return SMP_ERROR_OVERFLOW;

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

/**
 * \ingroup message
 * Initialize a get_input_value response message.
 *
 * @param[in] message the TupMessage
 * @param[in] effect_slot_id id of the effect to update values
 * @param[in] args an array of TupInputValueArgs
 * @param[in] n_args the number of elements in args
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_init_resp_input(TupMessage *message, uint8_t effect_slot_id,
        TupInputValueArgs *args, size_t n_args)
{
    int ret;
    size_t i, j;

    smp_message_set_id(message, TUP_MESSAGE_RESP_INPUT);

    ret = smp_message_set_uint8(message, 0, effect_slot_id);
    if (ret < 0)
        return ret;

    for (i = 0, j = 1; i < n_args; i++, j += 2) {
        ret = smp_message_set_uint8(message, j, args[i].input_id);
        if (ret < 0)
            return ret;

        ret = smp_message_set_int32(message, j + 1, args[i].input_value);
        if (ret < 0)
            return ret;
    }

    return 0;
}

/**
 * \ingroup message
 * Parse a get_input_value response message.
 *
 * @param[in] message the TupMessage
 * @param[out] effect_slot_id id of the effect to update values
 * @param[out] args an array of TupInputValueArgs
 * @param[in] size the size of args array
 *
 * @return the number of input value on success, a SmpError otherwise.
 */
int tup_message_parse_resp_input(TupMessage *message, uint8_t *effect_slot_id,
        TupInputValueArgs *args, size_t size)
{
    int n_params = 0;
    int ret;
    int i, j;

    if (smp_message_get_msgid(message) != TUP_MESSAGE_RESP_INPUT)
        return SMP_ERROR_BAD_MESSAGE;

    /* we use two args in SmpMessage for one parameter */
    n_params = (smp_message_n_args(message) - 1) / 2;
    if (size < (size_t) n_params)
        return SMP_ERROR_OVERFLOW;

    ret = smp_message_get_uint8(message, 0, effect_slot_id);
    if (ret < 0)
        return ret;

    for (i = 0, j = 1; i < n_params; i++, j += 2) {
        ret = smp_message_get_uint8(message, j, &args[i].input_id);
        if (ret < 0)
            return ret;

        ret = smp_message_get_int32(message, j + 1, &args[i].input_value);
        if (ret < 0)
            return ret;
    }

    return n_params;
}

/**
 * \ingroup message
 * Initialize a buildinfo response message with the given buildinfo.
 * Warning: due to smp limitation about string, version should exist as long as
 * message exist.
 *
 * @param[in] message the TupMessage
 * @param[in] buildinfo the buildinfo to report
 */
void tup_message_init_resp_buildinfo(TupMessage *message, const char *buildinfo)
{
    smp_message_set_id(message, TUP_MESSAGE_RESP_BUILDINFO);
    smp_message_set_cstring(message, 0, buildinfo);
}

/**
 * \ingroup message
 * Parse a buildinfo response message
 * Warning: buildinfo is not copied and so it is valid as long as message is
 * valid.
 *
 * @param[in] message the TupMessage
 * @param[out] buildinfo the buildinfo
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_message_parse_resp_buildinfo(TupMessage *message,
        const char **buildinfo)
{
    if (smp_message_get_msgid(message) != TUP_MESSAGE_RESP_BUILDINFO)
        return SMP_ERROR_BAD_MESSAGE;

    return smp_message_get_cstring(message, 0, buildinfo);
}

/**
 * \ingroup message
 * Initialize a message to get the system status.
 *
 * @param[in] message the TupMessage
 */
void tup_message_init_cmd_debug_get_system_status(TupMessage *message)
{
    smp_message_set_id(message, TUP_MESSAGE_CMD_DEBUG_GET_SYSTEM_STATUS);
}

/**
 * \ingroup message
 * Initialize a response message with the system status.
 *
 * @param[in] message the TupMessage
 * @param[in] status a filled TupDebugSystemStatus structure
 * @param[in] tasks an array of filled TupDebugTaskStatus
 * @param[in] n_tasks the number of tasks in tasks
 *
 * @return 0 on success, a negative errno value otherwise.
 */
int tup_message_init_resp_debug_system_status(TupMessage *message,
        TupDebugSystemStatus *status, TupDebugTaskStatus *tasks, size_t n_tasks)
{
    int sindex = 0;
    size_t i;
    int ret;

    smp_message_set_id(message, TUP_MESSAGE_RESP_DEBUG_SYSTEM_STATUS);

    /* first, serialize TupDebugSystemStatus */
    smp_message_set_uint64(message, sindex++, status->rtime);
    smp_message_set_uint32(message, sindex++, status->mem_total);
    smp_message_set_uint32(message, sindex++, status->mem_used);

    for (i = 0; i < n_tasks; i++) {
        ret = smp_message_set(message,
                6 * i + sindex + 0, SMP_TYPE_UINT32, tasks[i].id,
                6 * i + sindex + 1, SMP_TYPE_STRING, tasks[i].name,
                6 * i + sindex + 2, SMP_TYPE_UINT8, tasks[i].state,
                6 * i + sindex + 3, SMP_TYPE_UINT32, tasks[i].priority,
                6 * i + sindex + 4, SMP_TYPE_UINT64, tasks[i].time,
                6 * i + sindex + 5, SMP_TYPE_UINT32, tasks[i].rem_stack,
                -1);
        if (ret < 0)
            return ret;
    }

    return 0;
}

/**
 * \ingroup message
 * Parse a response message with the system status.
 *
 * @param[in] message the TupMessage
 * @param[in] status a TupDebugSystemStatus structure
 * @param[in] tasks an array of TupDebugTaskStatus
 * @param[in] n_tasks the number of tasks in tasks
 *
 * @return 0 on success, a negative errno value otherwise.
 */
int tup_message_parse_resp_debug_system_status(TupMessage *message,
        TupDebugSystemStatus *status, TupDebugTaskStatus *tasks, size_t n_tasks)
{
    size_t i;
    int ret;
    int n_msg_tasks;
    int sindex = 0;

    if (smp_message_get_msgid(message) != TUP_MESSAGE_RESP_DEBUG_SYSTEM_STATUS)
        return SMP_ERROR_BAD_MESSAGE;

    n_msg_tasks = (smp_message_n_args(message) - 3) / 6;
    if (n_tasks < (size_t) n_msg_tasks)
        return SMP_ERROR_OVERFLOW;

    ret = smp_message_get_uint64(message, sindex++, &status->rtime);
    if (ret < 0)
        return ret;

    ret = smp_message_get_uint32(message, sindex++, &status->mem_total);
    if (ret < 0)
        return ret;

    ret = smp_message_get_uint32(message, sindex++, &status->mem_used);
    if (ret < 0)
        return ret;

    for (i = 0; i < (size_t) n_msg_tasks; i++) {
        uint8_t tmp_state;

        ret = smp_message_get(message,
                6 * i + sindex + 0, SMP_TYPE_UINT32, &tasks[i].id,
                6 * i + sindex + 1, SMP_TYPE_STRING, &tasks[i].name,
                6 * i + sindex + 2, SMP_TYPE_UINT8, &tmp_state,
                6 * i + sindex + 3, SMP_TYPE_UINT32, &tasks[i].priority,
                6 * i + sindex + 4, SMP_TYPE_UINT64, &tasks[i].time,
                6 * i + sindex + 5, SMP_TYPE_UINT32, &tasks[i].rem_stack,
                -1);
        if (ret < 0)
            return ret;

        tasks[i].state = tmp_state;
    }

    return n_msg_tasks;
}
