/* libsmp
 * Copyright (C) 2017 Actronika SAS
 *     Author: Aurélien Zanelli <aurelien.zanelli@actronika.com>
 */

#ifndef LIBTUP_H
#define LIBTUP_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <libsmp.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef SmpMessage TupMessage;
typedef struct TupContext TupContext;

/* TupContext API */

typedef struct
{
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

int tup_context_send(TupContext *ctx, TupMessage *msg);
int tup_context_process_fd(TupContext *ctx);

/* TupMessage API */

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

    TUP_MESSAGE_RESP_VERSION = 100,
    TUP_MESSAGE_RESP_PARAMETER = 101,
    TUP_MESSAGE_RESP_SENSOR = 102,
} TupMessageType;

#define TUP_MESSAGE_TYPE(msg) ((TupMessageType) ((msg)->msgid))

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

void tup_message_init_get_parameter(TupMessage *message, uint8_t effect_id,
        uint8_t parameter_id);
int tup_message_parse_get_parameter(TupMessage *message, uint8_t *effect_id,
        uint8_t *parameter_id);

void tup_message_init_set_parameter(TupMessage *message, uint8_t effect_id,
        uint8_t parameter_id, uint32_t parameter_value);
int tup_message_parse_set_parameter(TupMessage *message, uint8_t *effect_id,
        uint8_t *parameter_id, uint32_t *parameter_value);

void tup_message_init_bind_effect(TupMessage *message, uint8_t effect_id,
        unsigned int binding_flags);
int tup_message_parse_bind_effect(TupMessage *message, uint8_t *effect_id,
        unsigned int *binding_flags);

void tup_message_init_get_sensor_value(TupMessage *message, uint8_t sensor_id);
int tup_message_parse_get_sensor_value(TupMessage *message, uint8_t *sensor_id);

void tup_message_init_set_sensor_value(TupMessage *message, uint8_t sensor_id,
        uint16_t sensor_value);
int tup_message_parse_set_sensor_value(TupMessage *message, uint8_t *sensor_id,
        uint16_t *sensor_value);

void tup_message_init_resp_version(TupMessage *message, const char *version);
int tup_message_parse_resp_version(TupMessage *message, const char **version);

void tup_message_init_resp_parameter(TupMessage *message, uint8_t effect_id,
        uint8_t parameter_id, uint32_t parameter_value);
int tup_message_parse_resp_parameter(TupMessage *message, uint8_t *effect_id,
        uint8_t *parameter_id, uint32_t *parameter_value);

void tup_message_init_resp_sensor(TupMessage *message, uint8_t sensor_id,
        uint16_t sensor_value);
int tup_message_parse_resp_sensor(TupMessage *message, uint8_t *sensor_id,
        uint16_t *sensor_value);

#ifdef __cplusplus
}
#endif

#endif
