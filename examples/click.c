#include <stdio.h>
#include <unistd.h>
#include <libtup.h>

#define EFFECT_ID_CLICK 0

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

    for (i = 0; i < 10; i++) {
        tup_message_init_set_input_value(&msg, 0, 0, 1010, -1);
        ret = tup_context_send(&ctx, &msg);
        if (ret < 0)
            goto send_fail;

        tup_message_clear(&msg);

        usleep(200000);

        tup_message_init_set_input_value(&msg, 0, 0, 0, -1);
        ret = tup_context_send(&ctx, &msg);
        if (ret < 0)
            goto send_fail;

        tup_message_clear(&msg);

        sleep(1);
    }

    tup_context_clear(&ctx);
    return 0;

send_fail:
    printf("failed to send message\n");
    tup_context_clear(&ctx);
    return 1;
}

