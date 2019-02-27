#define TUP_ENABLE_STATIC_API
#include <libtup.h>
#include <Arduino.h>

TUP_DEFINE_STATIC_CONTEXT(tup_ctx_storage, 128, 128, 64, 64);
TUP_DEFINE_STATIC_MESSAGE(tup_msg_storage, 8);

static TupContext *tup_ctx;

static void tup_msg_handler(TupContext *tup_ctx, TupMessage *msg, void *userdata)
{
    Serial.print("Message from Tactronik, id: ");
    Serial.println(TUP_MESSAGE_TYPE(msg));
}

TupCallbacks tup_callback = {
    .new_message_cb = tup_msg_handler
};

void setup()
{
    TupMessage *tup_msg;
    uint8_t effect_id = 0;
    uint16_t effect_database_id = 4;  /* heartbeat effect */
    int ret;

    Serial.begin(115200);
    Serial.println("Start");

    tup_ctx = tup_ctx_storage_create(&tup_callback, NULL);
    if (tup_ctx == NULL) {
        Serial.println("Failed to create Tup context");
        return;
    }

    tup_msg = tup_msg_storage_create();
    if (tup_msg == NULL) {
        Serial.println("Failed to create Tup message");
        return;
    }

    ret = tup_context_open(tup_ctx, "serial1");
    if (ret != 0) {
        Serial.print("Failed to open Tup context: ");
        Serial.println(ret);
        return;
    }

    tup_message_init_load(tup_msg, effect_id, effect_database_id);
    ret = tup_context_send(tup_ctx, tup_msg);
    if (ret != 0) {
        Serial.print("Send msg load faild: ");
        Serial.println(ret);
        return;
    }
    tup_message_clear(tup_msg);

    tup_message_init_bind_effect(tup_msg, effect_id, TUP_BINDING_FLAG_BOTH);
    ret = tup_context_send(tup_ctx, tup_msg);
    if (ret != 0) {
        Serial.print("Send msg bind faild: ");
        Serial.println(ret);
        return;
    }
    tup_message_clear(tup_msg);

    tup_message_init_play(tup_msg, effect_id);
    ret = tup_context_send(tup_ctx, tup_msg);
    if (ret != 0) {
        Serial.print("Send msg play faild: ");
        Serial.println(ret);
        return;
    }
    tup_message_clear(tup_msg);

    tup_message_init_set_parameter_simple(tup_msg, effect_id, 0, 100);
    ret = tup_context_send(tup_ctx, tup_msg);
    if (ret != 0) {
        Serial.print("Send msg set_param faild: ");
        Serial.println(ret);
        return;
    }
    tup_message_clear(tup_msg);
}

void loop()
{
    tup_context_process_fd(tup_ctx);

    /* Insert your code here. It must be non-blocking. */
}
