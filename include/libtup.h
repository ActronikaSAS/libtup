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
 */

#ifndef LIBTUP_H
#define LIBTUP_H

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <libsmp.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef SmpMessage TupMessage;
typedef struct TupContext TupContext;

/* TupContext API */

/**
 * \ingroup context
 * Callbacks structure
 */
typedef struct
{
    /** called when a new message has been received. */
    void (*new_message)(TupContext *ctx, TupMessage *message, void *userdata);
} TupCallbacks;

struct TupContext
{
    SmpSerialFrameContext sf_ctx;
    TupCallbacks cbs;

    void *userdata;
};

int tup_context_init(TupContext *ctx, const char *device, TupCallbacks *cbs,
        void *userdata);
void tup_context_clear(TupContext *ctx);

int tup_context_set_config(TupContext *ctx, SmpSerialFrameBaudrate baudrate,
        SmpSerialFrameParity parity, int flow_control);

intptr_t tup_context_get_fd(TupContext *ctx);

int tup_context_send(TupContext *ctx, TupMessage *msg);
int tup_context_process_fd(TupContext *ctx);

/* TupMessage API */

/**
 * \ingroup message
 * Type of the message
 */
typedef enum
{
    TUP_MESSAGE_ACK = 1,
    TUP_MESSAGE_ERROR = 2,
    TUP_MESSAGE_CMD_LOAD = 10,
    TUP_MESSAGE_CMD_PLAY = 11,
    TUP_MESSAGE_CMD_STOP = 12,
    TUP_MESSAGE_CMD_GET_VERSION = 13,
    TUP_MESSAGE_CMD_GET_PARAMETER = 14,
    TUP_MESSAGE_CMD_SET_PARAMETER = 15,
    TUP_MESSAGE_CMD_BIND_EFFECT = 16,
    TUP_MESSAGE_CMD_GET_SENSOR_VALUE = 17,
    TUP_MESSAGE_CMD_SET_SENSOR_VALUE = 18,
    TUP_MESSAGE_CMD_GET_BUILDINFO = 19,
    TUP_MESSAGE_CMD_ACTIVATE_INTERNAL_SENSORS = 20,
    TUP_MESSAGE_CMD_GET_INPUT_VALUE = 21,
    TUP_MESSAGE_CMD_SET_INPUT_VALUE = 22,

    TUP_MESSAGE_RESP_VERSION = 100,
    TUP_MESSAGE_RESP_PARAMETER = 101,
    TUP_MESSAGE_RESP_SENSOR = 102,
    TUP_MESSAGE_RESP_BUILDINFO = 103,
    TUP_MESSAGE_RESP_INPUT = 104
} TupMessageType;

/**
 * \ingroup message
 * Type of the message
 */
#define TUP_MESSAGE_TYPE(msg) ((TupMessageType) ((msg)->msgid))

/**
 * \ingroup message
 * Get/Set parameter argument structure
 */
typedef struct
{
    uint8_t parameter_id;        /**< parameter id */
    uint32_t parameter_value;    /**< parameter value */
} TupParameterArgs;

/**
 * \ingroup message
 * Get/Set sensor value structure
 */
typedef struct
{
    uint8_t sensor_id;        /**< sensor id */
    uint16_t sensor_value;    /**< sensor value */
} TupSensorValueArgs;

/**
 * \ingroup message
 * Get/Set input value structure
 */
typedef struct
{
    uint8_t input_id;        /**< input id */
    int32_t input_value;    /**< input value */
} TupInputValueArgs;

/**
 * \ingroup message
 * Binding flags
 */
typedef enum
{
    TUP_BINDING_FLAG_NONE = 0,      /**< Unbind */
    TUP_BINDING_FLAG_1 = 1,         /**< Bind to actuator 1 */
    TUP_BINDING_FLAG_2 = 1 << 1,    /**< Bind to actuator 2 */
    TUP_BINDING_FLAG_3 = 1 << 2,    /**< Bind to actuator 3 */
    TUP_BINDING_FLAG_4 = 1 << 3,    /**< Bind to actuator 4 */
    TUP_BINDING_FLAG_5 = 1 << 4,    /**< Bind to actuator 5 */
    TUP_BINDING_FLAG_6 = 1 << 5,    /**< Bind to actuator 6 */
    TUP_BINDING_FLAG_7 = 1 << 6,    /**< Bind to actuator 7 */
    TUP_BINDING_FLAG_8 = 1 << 7,    /**< Bind to actuator 8 */
    TUP_BINDING_FLAG_9 = 1 << 8,    /**< Bind to actuator 9 */
    TUP_BINDING_FLAG_10 = 1 << 9,   /**< Bind to actuator 10 */
    TUP_BINDING_FLAG_11 = 1 << 10,   /**< Bind to actuator 11 */
    TUP_BINDING_FLAG_12 = 1 << 11,   /**< Bind to actuator 12 */
    TUP_BINDING_FLAG_13 = 1 << 12,   /**< Bind to actuator 13 */
    TUP_BINDING_FLAG_14 = 1 << 13,   /**< Bind to actuator 14 */
    TUP_BINDING_FLAG_15 = 1 << 14,   /**< Bind to actuator 15 */
    TUP_BINDING_FLAG_16 = 1 << 15,   /**< Bind to actuator 16 */
} TupBindingFlags;

/**
 * \ingroup message
 * Bind to both actuators 1 and 2.
 */
#define TUP_BINDING_FLAG_BOTH (TUP_BINDING_FLAG_1 | TUP_BINDING_FLAG_2)

/* TUP messages */
void tup_message_clear(TupMessage *message);

void tup_message_init_ack(TupMessage *message, TupMessageType cmd);
int tup_message_parse_ack(TupMessage *message, TupMessageType *cmd);

void tup_message_init_error(TupMessage *message, TupMessageType cmd,
        uint32_t error);
int tup_message_parse_error(TupMessage *message, TupMessageType *cmd,
        uint32_t *error);

void tup_message_init_load(TupMessage *message, uint8_t effect_id,
        uint16_t bank_id);
int tup_message_parse_load(TupMessage *message, uint8_t *effect_id,
        uint16_t *bank_id);

void tup_message_init_play(TupMessage *message, uint8_t effect_id);
int tup_message_parse_play(TupMessage *message, uint8_t *effect_id);

void tup_message_init_stop(TupMessage *message, uint8_t effect_id);
int tup_message_parse_stop(TupMessage *message, uint8_t *effect_id);

void tup_message_init_get_version(TupMessage *message);

int tup_message_init_get_parameter(TupMessage *message, uint8_t effect_id,
        int parameter_id, ...);
int tup_message_init_get_parameter_valist(TupMessage *message,
        uint8_t effect_id, int parameter_id, va_list varargs);
void tup_message_init_get_parameter_simple(TupMessage *message,
        uint8_t effect_id, uint8_t parameter_id);
int tup_message_init_get_parameter_array(TupMessage *message,
        uint8_t effect_id, uint8_t *parameter_ids, size_t n_parameters);
int tup_message_parse_get_parameter(TupMessage *message, uint8_t *effect_id,
       uint8_t *parameter_ids, size_t size);

int tup_message_init_set_parameter(TupMessage *message, uint8_t effect_id,
        int parameter_id, uint32_t parameter_value, ...);
int tup_message_init_set_parameter_valist(TupMessage *message,
        uint8_t effect_id, int parameter_id, uint32_t parameter_value,
        va_list varargs);
void tup_message_init_set_parameter_simple(TupMessage *message,
        uint8_t effect_id, uint8_t parameter_id, uint32_t parameter_value);
int tup_message_init_set_parameter_array(TupMessage *message,
        uint8_t effect_id, TupParameterArgs *params, size_t n_params);
int tup_message_parse_set_parameter(TupMessage *message,
        uint8_t *effect_id, TupParameterArgs *params, size_t size);

void tup_message_init_bind_effect(TupMessage *message, uint8_t effect_id,
        unsigned int binding_flags);
int tup_message_parse_bind_effect(TupMessage *message, uint8_t *effect_id,
        unsigned int *binding_flags);

int tup_message_init_get_sensor_value(TupMessage *message, int sensor_id, ...);
int tup_message_init_get_sensor_value_valist(TupMessage *message, int sensor_id,
        va_list varargs);
void tup_message_init_get_sensor_value_simple(TupMessage *message,
        uint8_t sensor_id);
int tup_message_init_get_sensor_value_array(TupMessage *message,
        uint8_t *sensor_ids, size_t n_sensors);
int tup_message_parse_get_sensor_value(TupMessage *message,
        uint8_t *sensor_ids, size_t size);

int tup_message_init_set_sensor_value(TupMessage *message, int sensor_id,
        int sensor_value, ...);
int tup_message_init_set_sensor_value_valist(TupMessage *message, int sensor_id,
        int sensor_value, va_list varargs);
void tup_message_init_set_sensor_value_simple(TupMessage *message,
        int sensor_id, uint16_t sensor_value);
int tup_message_init_set_sensor_value_array(TupMessage *message,
        TupSensorValueArgs *args, size_t n_args);
int tup_message_parse_set_sensor_value(TupMessage *message,
        TupSensorValueArgs *args, size_t size);

int tup_message_init_get_input_value(TupMessage *message,
        int effect_slot_id, int input_id, ...);
int tup_message_init_get_input_value_valist(TupMessage *message,
        int effect_slot_id, int input_id, va_list varargs);
void tup_message_init_get_input_value_simple(TupMessage *message,
        uint8_t effect_slot_id, uint8_t input_id);
int tup_message_init_get_input_value_array(TupMessage *message,
        uint8_t effect_slot_id, uint8_t *input_ids, size_t n_inputs);
int tup_message_parse_get_input_value(TupMessage *message,
        uint8_t *effect_slot_id, uint8_t *input_ids, size_t size);

int tup_message_init_set_input_value(TupMessage *message,
        int effect_slot_id, int input_id, int input_value, ...);
int tup_message_init_set_input_value_valist(TupMessage *message,
        int effect_slot_id, int input_id, int input_value, va_list varargs);
void tup_message_init_set_input_value_simple(TupMessage *message,
        uint8_t effect_slot_id, uint8_t input_id, int input_value);
int tup_message_init_set_input_value_array(TupMessage *message,
        uint8_t effect_slot_id, TupInputValueArgs *args, size_t n_args);
int tup_message_parse_set_input_value(TupMessage *message,
        uint8_t *effect_slot_id, TupInputValueArgs *args, size_t size);

void tup_message_init_get_buildinfo(TupMessage *message);

void tup_message_init_activate_internal_sensors(TupMessage *message,
        uint8_t state);
int tup_message_parse_activate_internal_sensors(TupMessage *message,
        uint8_t *state);

void tup_message_init_resp_version(TupMessage *message, const char *version);
int tup_message_parse_resp_version(TupMessage *message, const char **version);

int tup_message_init_resp_parameter(TupMessage *message, uint8_t effect_id,
        TupParameterArgs *args, size_t n_args);
int tup_message_parse_resp_parameter(TupMessage *message, uint8_t *effect_id,
        TupParameterArgs *args, size_t size);

int tup_message_init_resp_sensor(TupMessage *message, TupSensorValueArgs *args,
        size_t n_args);
int tup_message_parse_resp_sensor(TupMessage *message, TupSensorValueArgs *args,
        size_t size);

int tup_message_init_resp_input(TupMessage *message, uint8_t effect_slot_id,
        TupInputValueArgs *args, size_t n_args);
int tup_message_parse_resp_input(TupMessage *message, uint8_t *effect_slot_id,
        TupInputValueArgs *args, size_t size);

void tup_message_init_resp_buildinfo(TupMessage *message, const char *buildinfo);
int tup_message_parse_resp_buildinfo(TupMessage *message,
        const char **buildinfo);

#ifdef __cplusplus
}
#endif

#endif
