#include <libtup.h>
#include <Arduino.h>

TupContext tup_context;
TupMessage tup_msg;

static void tup_msg_handler(TupContext *tup_ctx, TupMessage *msg, void *userdata)
{
    Serial.print("Message from Tactronik, id: ");
    Serial.println(TUP_MESSAGE_TYPE(msg));
}

TupCallbacks tup_callback = {
    .new_message = tup_msg_handler
};

void setup()
{
    uint8_t effect_id = 0;
    uint16_t effect_database_id = 1;
    int ret;

    Serial.begin(115200);
    Serial.println("Start");

    ret = tup_context_init(&tup_context, "serial1", &tup_callback, NULL);
    if (ret != 0) {
        Serial.print("Init failed: ");
        Serial.println(ret);
        return;
    }

    tup_message_init_load(&tup_msg, effect_id, effect_database_id);
    ret = tup_context_send(&tup_context, &tup_msg);
    if (ret != 0) {
        Serial.print("Send msg load faild: ");
        Serial.println(ret);
        return;
    }
    tup_message_clear(&tup_msg);

    tup_message_init_bind_effect(&tup_msg, effect_id, TUP_BINDING_FLAG_BOTH);
    ret = tup_context_send(&tup_context, &tup_msg);
    if (ret != 0) {
        Serial.print("Send msg bind faild: ");
        Serial.println(ret);
        return;
    }
    tup_message_clear(&tup_msg);

    tup_message_init_play(&tup_msg, effect_id);
    ret = tup_context_send(&tup_context, &tup_msg);
    if (ret != 0) {
        Serial.print("Send msg play faild: ");
        Serial.println(ret);
        return;
    }
    tup_message_clear(&tup_msg);

    tup_message_init_set_parameter_simple(&tup_msg, effect_id, 0, 100);
    ret = tup_context_send(&tup_context, &tup_msg);
    if (ret != 0) {
        Serial.print("Send msg set_param faild: ");
        Serial.println(ret);
        return;
    }
    tup_message_clear(&tup_msg);
}

void loop()
{
    tup_context_process_fd(&tup_context);

    /* Insert your code here. It must be non-blocking. */
}
