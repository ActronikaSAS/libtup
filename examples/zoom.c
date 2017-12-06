#include <stdio.h>
#include <unistd.h>
#include <libtup.h>

#define EFFECT_ID_SCROLL 0x10
#define SENSOR_ID_X1 2
#define SENSOR_ID_Y1 3
#define SENSOR_ID_X2 5
#define SENSOR_ID_Y2 6
#define SENSOR_ID_N_FINGERS 18


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

    /* Load effect scroll to slot 0 */
    tup_message_init_load(&msg, 0, EFFECT_ID_SCROLL);
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

    tup_message_init_set_sensor_value(&msg, SENSOR_ID_N_FINGERS, 2, -1);
    ret = tup_context_send(&ctx, &msg);
    if (ret < 0)
        goto send_fail;

    tup_message_clear(&msg);

    // Set finger 1 position
    tup_message_init_set_sensor_value(&msg, SENSOR_ID_X1, 0, SENSOR_ID_Y1, 0, -1);
    ret = tup_context_send(&ctx, &msg);
    if (ret < 0)
        goto send_fail;

    tup_message_clear(&msg);

    for (i = 0; i < 1000; i+= 50) {
        // set finger 2 position
        tup_message_init_set_sensor_value(&msg, SENSOR_ID_X2, i, SENSOR_ID_Y2, i, -1);
        ret = tup_context_send(&ctx, &msg);
        if (ret < 0)
            goto send_fail;

        tup_message_clear(&msg);

        usleep(50000);
    }

    tup_context_clear(&ctx);
    return 0;

send_fail:
    printf("failed to send message\n");
    tup_context_clear(&ctx);
    return 1;
}

