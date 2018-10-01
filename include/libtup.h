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

#ifdef TUP_ENABLE_STATIC_API
#define SMP_ENABLE_STATIC_API
#endif
#include <libsmp.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Use to declare public API */
#ifdef TUP_EXPORT_API
#   ifdef _WIN32
#       define TUP_API __declspec(dllexport)
#   else
#       define TUP_API __attribute__((visibility("default")))
#   endif
#else
#   define TUP_API
#endif

typedef void TupMessage;
typedef SmpContext TupContext;

/* TupContext API */

/**
 * \ingroup context
 * Callbacks structure
 */
typedef struct
{
    /** called when a new message has been received. */
    void (*new_message_cb)(TupContext *ctx, TupMessage *message, void *userdata);
    void (*error_cb)(TupContext *ctx, SmpError error, void *userdata);
} TupCallbacks;

TUP_API TupContext *tup_context_new(TupCallbacks *cbs, void *userdata);
TUP_API void tup_context_free(TupContext *ctx);

TUP_API int tup_context_open(TupContext *ctx, const char *device);
TUP_API void tup_context_close(TupContext *ctx);

TUP_API int tup_context_set_config(TupContext *ctx, SmpSerialBaudrate baudrate,
                SmpSerialParity parity, int flow_control);

TUP_API intptr_t tup_context_get_fd(TupContext *ctx);

TUP_API int tup_context_send(TupContext *ctx, TupMessage *msg);
TUP_API int tup_context_process_fd(TupContext *ctx);
TUP_API int tup_context_wait_and_process(TupContext *ctx, int timeout_ms);

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
    TUP_MESSAGE_RESP_INPUT = 104,

    TUP_MESSAGE_CMD_DEBUG_GET_SYSTEM_STATUS = 200,
    TUP_MESSAGE_RESP_DEBUG_SYSTEM_STATUS = 201,
} TupMessageType;

/**
 * \ingroup message
 * Type of the message
 */
#define TUP_MESSAGE_TYPE(msg) (tup_message_get_type(msg))

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

/**
 * \ingroup message
 * The task state
 */
typedef enum
{
    TUP_DEBUG_TASK_STATE_NONE = 0,     /**< none */
    TUP_DEBUG_TASK_STATE_READY,        /**< task is ready to be run */
    TUP_DEBUG_TASK_STATE_RUNNING,      /**< task is running */
    TUP_DEBUG_TASK_STATE_BLOCKED,      /**< task is blocked */
    TUP_DEBUG_TASK_STATE_SUSPENDED,    /**< task is suspended */
    TUP_DEBUG_TASK_STATE_DELETED,      /**< task is deleted */
} TupDebugTaskState;

/**
 * \ingroup message
 * The task status
 */
typedef struct
{
    uint32_t id;                /**< id of the task (unique) */
    const char *name;           /**< name of the task */
    TupDebugTaskState state;    /**< state of the task */
    uint32_t priority;          /**< priority of the task */
    uint64_t time;              /**< running time of the task in microseconds */
    uint32_t rem_stack;         /**< the remaining stack space in bytes */
} TupDebugTaskStatus;

/**
 * \ingroup message
 * The system status
 */
typedef struct
{
    uint64_t rtime;        /**< run time since the boot in microseconds */
    uint32_t mem_total;    /**< the total available memory in the heap */
    uint32_t mem_used;     /**< the total used memory */
} TupDebugSystemStatus;

/* TUP messages */
TUP_API TupMessage *tup_message_new(void);
TUP_API void tup_message_free(TupMessage *message);
TUP_API TupMessageType tup_message_get_type(TupMessage *message);

TUP_API void tup_message_clear(TupMessage *message);

TUP_API void tup_message_init_ack(TupMessage *message, TupMessageType cmd);
TUP_API int tup_message_parse_ack(TupMessage *message, TupMessageType *cmd);

TUP_API void tup_message_init_error(TupMessage *message, TupMessageType cmd,
                uint32_t error);
TUP_API int tup_message_parse_error(TupMessage *message, TupMessageType *cmd,
                uint32_t *error);

TUP_API void tup_message_init_load(TupMessage *message, uint8_t effect_id,
                uint16_t bank_id);
TUP_API int tup_message_parse_load(TupMessage *message, uint8_t *effect_id,
                uint16_t *bank_id);

TUP_API void tup_message_init_play(TupMessage *message, uint8_t effect_id);
TUP_API int tup_message_parse_play(TupMessage *message, uint8_t *effect_id);

TUP_API void tup_message_init_stop(TupMessage *message, uint8_t effect_id);
TUP_API int tup_message_parse_stop(TupMessage *message, uint8_t *effect_id);

TUP_API void tup_message_init_get_version(TupMessage *message);

TUP_API int tup_message_init_get_parameter(TupMessage *message,
                uint8_t effect_id, int parameter_id, ...);
TUP_API int tup_message_init_get_parameter_valist(TupMessage *message,
                uint8_t effect_id, int parameter_id, va_list varargs);
TUP_API void tup_message_init_get_parameter_simple(TupMessage *message,
                uint8_t effect_id, uint8_t parameter_id);
TUP_API int tup_message_init_get_parameter_array(TupMessage *message,
                uint8_t effect_id, uint8_t *parameter_ids, size_t n_parameters);
TUP_API void tup_message_init_get_parameter_set_effect_id(TupMessage *message,
                uint8_t effect_id);
TUP_API int tup_message_init_get_parameter_set_parameter_id(TupMessage *message,
                unsigned int index, uint8_t parameter_id);
TUP_API int tup_message_parse_get_parameter(TupMessage *message,
                uint8_t *effect_id, uint8_t *parameter_ids, size_t size);

TUP_API int tup_message_init_set_parameter(TupMessage *message,
                uint8_t effect_id, int parameter_id, uint32_t parameter_value,
                ...);
TUP_API int tup_message_init_set_parameter_valist(TupMessage *message,
                uint8_t effect_id, int parameter_id, uint32_t parameter_value,
                va_list varargs);
TUP_API void tup_message_init_set_parameter_simple(TupMessage *message,
                uint8_t effect_id, uint8_t parameter_id,
                uint32_t parameter_value);
TUP_API int tup_message_init_set_parameter_array(TupMessage *message,
                uint8_t effect_id, TupParameterArgs *params, size_t n_params);
TUP_API int tup_message_parse_set_parameter(TupMessage *message,
                uint8_t *effect_id, TupParameterArgs *params, size_t size);

TUP_API void tup_message_init_bind_effect(TupMessage *message,
                uint8_t effect_id, unsigned int binding_flags);
TUP_API int tup_message_parse_bind_effect(TupMessage *message,
                uint8_t *effect_id, unsigned int *binding_flags);

TUP_API int tup_message_init_get_sensor_value(TupMessage *message,
                int sensor_id, ...);
TUP_API int tup_message_init_get_sensor_value_valist(TupMessage *message,
                int sensor_id, va_list varargs);
TUP_API void tup_message_init_get_sensor_value_simple(TupMessage *message,
                uint8_t sensor_id);
TUP_API int tup_message_init_get_sensor_value_array(TupMessage *message,
                uint8_t *sensor_ids, size_t n_sensors);
TUP_API int tup_message_parse_get_sensor_value(TupMessage *message,
                uint8_t *sensor_ids, size_t size);

TUP_API int tup_message_init_set_sensor_value(TupMessage *message,
                int sensor_id, int sensor_value, ...);
TUP_API int tup_message_init_set_sensor_value_valist(TupMessage *message,
                int sensor_id, int sensor_value, va_list varargs);
TUP_API void tup_message_init_set_sensor_value_simple(TupMessage *message,
                int sensor_id, uint16_t sensor_value);
TUP_API int tup_message_init_set_sensor_value_array(TupMessage *message,
                TupSensorValueArgs *args, size_t n_args);
TUP_API int tup_message_parse_set_sensor_value(TupMessage *message,
                TupSensorValueArgs *args, size_t size);

TUP_API int tup_message_init_get_input_value(TupMessage *message,
                int effect_slot_id, int input_id, ...);
TUP_API int tup_message_init_get_input_value_valist(TupMessage *message,
                int effect_slot_id, int input_id, va_list varargs);
TUP_API void tup_message_init_get_input_value_simple(TupMessage *message,
                uint8_t effect_slot_id, uint8_t input_id);
TUP_API int tup_message_init_get_input_value_array(TupMessage *message,
                uint8_t effect_slot_id, uint8_t *input_ids, size_t n_inputs);
TUP_API int tup_message_parse_get_input_value(TupMessage *message,
                uint8_t *effect_slot_id, uint8_t *input_ids, size_t size);

TUP_API int tup_message_init_set_input_value(TupMessage *message,
                int effect_slot_id, int input_id, int input_value, ...);
TUP_API int tup_message_init_set_input_value_valist(TupMessage *message,
                int effect_slot_id, int input_id, int input_value,
                va_list varargs);
TUP_API void tup_message_init_set_input_value_simple(TupMessage *message,
                uint8_t effect_slot_id, uint8_t input_id, int input_value);
TUP_API int tup_message_init_set_input_value_array(TupMessage *message,
                uint8_t effect_slot_id, TupInputValueArgs *args, size_t n_args);
TUP_API int tup_message_parse_set_input_value(TupMessage *message,
                uint8_t *effect_slot_id, TupInputValueArgs *args, size_t size);

TUP_API void tup_message_init_get_buildinfo(TupMessage *message);

TUP_API void tup_message_init_activate_internal_sensors(TupMessage *message,
                uint8_t state);
TUP_API int tup_message_parse_activate_internal_sensors(TupMessage *message,
                uint8_t *state);

TUP_API void tup_message_init_resp_version(TupMessage *message,
                const char *version);
TUP_API int tup_message_parse_resp_version(TupMessage *message,
                const char **version);

TUP_API int tup_message_init_resp_parameter(TupMessage *message,
                uint8_t effect_id, TupParameterArgs *args, size_t n_args);
TUP_API int tup_message_parse_resp_parameter(TupMessage *message,
                uint8_t *effect_id, TupParameterArgs *args, size_t size);
TUP_API int tup_message_parse_resp_parameter_get_effect_id(TupMessage *message,
                uint8_t *effect_id);
TUP_API int tup_message_parse_resp_parameter_get_parameter(TupMessage *message,
                unsigned int index, TupParameterArgs *arg);

TUP_API int tup_message_init_resp_sensor(TupMessage *message,
                TupSensorValueArgs *args, size_t n_args);
TUP_API int tup_message_parse_resp_sensor(TupMessage *message,
                TupSensorValueArgs *args, size_t size);

TUP_API int tup_message_init_resp_input(TupMessage *message,
                uint8_t effect_slot_id, TupInputValueArgs *args, size_t n_args);
TUP_API int tup_message_parse_resp_input(TupMessage *message,
                uint8_t *effect_slot_id, TupInputValueArgs *args, size_t size);

TUP_API void tup_message_init_resp_buildinfo(TupMessage *message,
                const char *buildinfo);
TUP_API int tup_message_parse_resp_buildinfo(TupMessage *message,
                const char **buildinfo);

TUP_API void tup_message_init_cmd_debug_get_system_status(TupMessage *message);

TUP_API int tup_message_init_resp_debug_system_status(TupMessage *message,
                TupDebugSystemStatus *status, TupDebugTaskStatus *tasks,
                size_t n_tasks);
TUP_API int tup_message_parse_resp_debug_system_status(TupMessage *message,
                TupDebugSystemStatus *status, TupDebugTaskStatus *tasks,
                size_t n_tasks);

#ifdef TUP_ENABLE_STATIC_API
/* For now TupContext and TupMessage needs no storage so */
typedef void TupStaticContext;
typedef void TupStaticMessage;

/**
 * \ingroup context
 * Helper macro to completely define a TupContext using static storage with
 * provided buffer size.
 * It defines a function which should be called in your code. The function
 * name is the concatenation of provided 'name' and '_create' and return
 * a TupContext*. So the prototype is:
 * `static TupContext *name_create(const TupCallbacks *cbs, void *userdata)`.
 * See tup_context_new() for parameters description.
 *
 * @param[in] name the name of the context
 * @param[in] serial_rx_bufsize the size of the serial rx buffer
 * @param[in] serial_tx_bufsize the size of the serial tx buffer
 * @param[in] msg_tx_bufsize the size of the message buffer for tx
 * @param[in] msg_rx_values_size the maximum number of values in the rx message
 */
#define TUP_DEFINE_STATIC_CONTEXT(name, serial_rx_bufsize, serial_tx_bufsize,  \
        msg_tx_bufsize, msg_rx_values_size)                                    \
SMP_DEFINE_STATIC_CONTEXT(name##_smp, serial_rx_bufsize, serial_tx_bufsize,    \
            msg_tx_bufsize, msg_rx_values_size)                                \
static TupContext* name##_create(const TupCallbacks *cbs, void *userdata)      \
{                                                                              \
    SmpContext *ctx;                                                           \
    SmpEventCallbacks scbs;                                                    \
                                                                               \
    scbs.new_message_cb =                                                      \
        (void (*)(SmpContext* , SmpMessage *, void *)) cbs->new_message_cb;    \
    scbs.error_cb = cbs->error_cb;                                             \
                                                                               \
    ctx = name##_smp_create(&scbs, userdata);                                  \
    return tup_context_new_from_static(NULL, 0, ctx, NULL, NULL);              \
}

/**
 * \ingroup message
 * Helper macro to define a TupMessage using static storage.
 * It defines a function to be called in your code by concatenating `name` and
 * `_create()` and return a pointer to the TupMessage.
 * The created function takes the msg id as the first parameter.
 *
 * @param[in] name the name to use
 * @param[in] max_values the maximum number of values in the message
 */
#define TUP_DEFINE_STATIC_MESSAGE(name, max_values)                           \
SMP_DEFINE_STATIC_MESSAGE(name##_smp, max_values)                             \
static TupMessage* name##_create()                                            \
{                                                                             \
    return tup_message_new_from_static(NULL, 0, name##_smp_create(0));        \
}

TUP_API TupContext *tup_context_new_from_static(TupStaticContext *sctx,
                size_t struct_size, SmpContext *smp_ctx,
                const TupCallbacks *cbs, void *userdata);

TUP_API TupMessage *tup_message_new_from_static(TupStaticMessage *smsg,
                size_t struct_size, SmpMessage *smp_msg);

#endif

#ifdef __cplusplus
}
#endif

#endif
