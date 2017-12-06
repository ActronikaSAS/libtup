#include <stdio.h>
#include <unistd.h>
#include <libtup.h>

#define EFFECT_ID_CLICK 0x01
#define SENSOR_ID_FORCE 14

static void on_new_message(TupContext *ctx, TupMessage *message, void *userdata)
{
    printf("New message received, id: %d\n", TUP_MESSAGE_TYPE(message));
}

int main(int argc, char *argv[])
{
    TupContext ctx;
    TupCallbacks cbs = {
        .new_message = on_new_message
    };
    TupMessage msg;
    int ret;
    int i;

    /* Initialize a TupContext using /dev/ttyUSB0 as serial device */
    ret = tup_context_init(&ctx, "/dev/ttyUSB0", &cbs, NULL);
    if (ret < 0) {
        printf("failed to initialize TupContext\n");
        return 1;
    }

    /* Load effect click to slot 0 */
    tup_message_init_load(&msg, 0, EFFECT_ID_CLICK);
    ret = tup_context_send(&ctx, &msg);
    if (ret < 0)
        goto send_fail;

    tup_message_clear(&msg);

    /* Bind effect in slot 0 to both actuators */
    tup_message_init_bind_effect(&msg, 0, TUP_BINDING_FLAG_BOTH);
    ret = tup_context_send(&ctx, &msg);
    if (ret < 0)
        goto send_fail;

    tup_message_clear(&msg);

    /* Play effect in slot 0 */
    tup_message_init_play(&msg, 0);
    ret = tup_context_send(&ctx, &msg);
    if (ret < 0)
        goto send_fail;

    tup_message_clear(&msg);

    for (int j = 0; j < 2; j++) {
        for (i = 0; i < 1000; i+= 50) {
            tup_message_init_set_sensor_value(&msg, SENSOR_ID_FORCE, i, -1);
            ret = tup_context_send(&ctx, &msg);
            if (ret < 0)
                goto send_fail;

            tup_message_clear(&msg);

            usleep(100000);
        }

        for (i = 1000; i > 200; i-= 50) {
            tup_message_init_set_sensor_value(&msg, SENSOR_ID_FORCE, i, -1);
            ret = tup_context_send(&ctx, &msg);
            if (ret < 0)
                goto send_fail;

            tup_message_clear(&msg);

            usleep(100000);
        }
    }
    sleep(1);

    tup_message_init_set_sensor_value(&msg, SENSOR_ID_FORCE, 0, -1);
    ret = tup_context_send(&ctx, &msg);
    if (ret < 0)
        goto send_fail;

    tup_message_clear(&msg);

    tup_context_clear(&ctx);
    return 0;

send_fail:
    printf("failed to send message\n");
    tup_context_clear(&ctx);
    return 1;
}

