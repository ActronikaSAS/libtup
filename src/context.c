/* libsmp
 * Copyright (C) 2017 Actronika SAS
 *     Author: Aurélien Zanelli <aurelien.zanelli@actronika.com>
 */

#include "libtup.h"
#include <stdio.h>

static void on_sf_new_frame(uint8_t *frame, size_t size, void *userdata)
{
    TupContext *ctx = userdata;
    TupMessage msg;
    int ret;

    ret = smp_message_init_from_buffer(&msg, frame, size);
    if (ret < 0) {
        fprintf(stderr, "failed to init message from frame\n");
        return;
    }

    ctx->cbs.new_message(ctx, &msg, ctx->userdata);
}

static void on_sf_error(SmpSerialFrameError error, void *userdata)
{
    fprintf(stderr, "serial frame error: %d\n", error);
}

/* API */

int tup_context_init(TupContext *ctx, const char *device, TupCallbacks *cbs,
        void *userdata)
{
    const SmpSerialFrameDecoderCallbacks sf_cbs = {
        .new_frame = on_sf_new_frame,
        .error = on_sf_error
    };

    ctx->cbs = *cbs;
    ctx->userdata = userdata;

    return smp_serial_frame_init(&ctx->sf_ctx, device, &sf_cbs, ctx);
}

void tup_context_clear(TupContext *ctx)
{
    smp_serial_frame_deinit(&ctx->sf_ctx);
}

int tup_context_send(TupContext *ctx, TupMessage *msg)
{
    uint8_t buf[512];
    int ret;

    ret = smp_message_encode(msg, buf, sizeof(buf));
    if (ret < 0)
        return ret;

    return smp_serial_frame_send(&ctx->sf_ctx, buf, ret);
}

int tup_context_process_fd(TupContext *ctx)
{
    return smp_serial_frame_process_recv_fd(&ctx->sf_ctx);
}
