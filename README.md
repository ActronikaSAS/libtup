# Tactronik UART Protocol library

This library contains message definition for tactronik UART API and helpers
to send/receive them.

## Basic usage

This very basic exemple demonstrates how to ask Tactronik to play effect.
```c
#include <stdio.h>
#include <unistd.h>
#include <libtup.h>

int ack_received = 0;

static void on_new_message(TupContext *ctx, TupMessage *message, void *userdata)
{
    printf("New message received, id: %d\n", TUP_MESSAGE_TYPE(message));
    if (TUP_MESSAGE_TYPE(message) == TUP_MESSAGE_ACK ||
            TUP_MESSAGE_TYPE(message) == TUP_MESSAGE_ERROR) {
        TupMessageType cmd;

        tup_message_parse_ack(message, &cmd);

        /* stop when we receive the ack or the error for the play command */
        if (cmd == TUP_MESSAGE_CMD_PLAY)
            ack_received = 1;
    }
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

    /* Load effect 4 to slot 0 */
    tup_message_init_load(&msg, 0, 4);
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

    /* wait for ack or timeout after 5 seconds */
    for (i = 0; i < 5 && !ack_received; i++) {
        tup_context_process_fd(&ctx);
        sleep(1);
    }

    if (!ack_received)
        printf("timeout while waiting for ack\n");

    tup_context_clear(&ctx);
    return 0;

send_fail:
    printf("failed to send message\n");
    tup_context_clear(&ctx);
    return 1;
}
```

First, a `TupContext` is initialized with serial device `/dev/ttyUSB0` and
`new_message` callback:
```c
    ret = tup_context_init(&ctx, "/dev/ttyUSB0", &cbs, NULL);
```

Then we load the effect number 4 to slot 0. Indeed due to memory limitation,
effect should be loaded to a slot before use. The number of slot available
depends on the configuration:
```c
    tup_message_init_load(&msg, 0, 4);
    ret = tup_context_send(&ctx, &msg);
```

After that, the effect is binding to both actuator 1 and 2 using the bind
message:
```c
tup_message_init_bind_effect(&msg, 0, TUP_BINDING_FLAG_BOTH);
ret = tup_context_send(&ctx, &msg);
```

At the end, we play the effect:
```c
tup_message_init_play(&msg, 0);
ret = tup_context_send(&ctx, &msg);
```

and we wait for ack to be received (or timeout) by using
`tup_context_process_fd()` which will call our `on_new_message` callback every
time a message is received:
```c
for (i = 0; i < 5 && !ack_received; i++) {
    tup_context_process_fd(&ctx);
    sleep(1);
}
```
