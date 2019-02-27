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
 * \defgroup context Context
 */

#ifndef TUP_ENABLE_STATIC_API
#define TUP_ENABLE_STATIC_API
#endif

#include "libtup.h"
#include <stdio.h>
#include <stdlib.h>

/* API */

typedef void (*SmpNewMessageCb)(SmpContext* , SmpMessage *, void *);

/**
 * \ingroup context
 * Create an initialize a new TupContext
 *
 * @param[in] cbs pointer to a callback structure
 * @param[in] userdata userdata to pass in callbacks
 *
 * @return a TupContext on success, NULL otherwise.
 */
TupContext *tup_context_new(TupCallbacks *cbs, void *userdata)
{
    SmpEventCallbacks scbs = {
        .new_message_cb = (SmpNewMessageCb) cbs->new_message_cb,
        .error_cb = cbs->error_cb,
    };

    return smp_context_new(&scbs, userdata);
}

/**
 * \ingroup context
 * Create a new TupContext object from a static storage.
 * @warning for now the TupContext is only a typedef of SmpContext so the
 * TupCallbacks should be converted and set to the SmpContext. This function
 * takes the callbacks only for future compatibility purpose.
 * Also you can pass NULL and 0 for TupStaticContext and struct_size for now.
 *
 * @param[in] sctx a TupStaticContext (not used)
 * @param[in] struct_size the size of sctx (not used)
 * @param[in] smp_ctx a SmpContext to use
 *
 * @return a SmpContext or NULL on error.
 */
TupContext *tup_context_new_from_static(TupStaticContext *sctx,
        size_t struct_size, SmpContext *smp_ctx, const TupCallbacks *cbs,
        void *userdata)
{
    return smp_ctx;
}

/**
 * \ingroup context
 * Free a TupContext.
 *
 * @param[in] ctx the TupContext to free
 */
void tup_context_free(TupContext *ctx)
{
    smp_context_free(ctx);
}

/**
 * \ingroup context
 * Open the provided serial device and use it in the given context.
 *
 * @param[in] ctx the TupContext
 * @param[in] device path to the serial device to use
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_context_open(TupContext *ctx, const char *device)
{
    return smp_context_open(ctx, device);
}

/**
 * \ingroup context
 * Close the context, releasing the attached serial device.
 *
 * @param[in] ctx the TupContext
 */
void tup_context_close(TupContext *ctx)
{
    return smp_context_close(ctx);
}

/**
 * \ingroup context
 * Set serial config. Depending on the system, it could be not implemented.
 *
 * @param[in] ctx the TupContext to initialize
 * @param[in] baudrate the baudrate
 * @param[in] parity the parity configuration
 * @param[in] flow_control 1 to enable flow control, 0 to disable
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_context_set_config(TupContext *ctx, SmpSerialBaudrate baudrate,
        SmpSerialParity parity, int flow_control)
{
    return smp_context_set_serial_config(ctx, baudrate, parity, flow_control);
}

/**
 * \ingroup context
 * Get the file descriptor of the opened serial device.
 *
 * @param[in] ctx the TupContext
 *
 * @return the fd on success, a SmpError otherwise.
 */
intptr_t tup_context_get_fd(TupContext *ctx)
{
    return smp_context_get_fd(ctx);
}

/**
 * \ingroup context
 * Send a message to the peer
 *
 * @param[in] ctx the TupContext
 * @param[in] msg the TupMessage to send
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_context_send(TupContext *ctx, TupMessage *msg)
{
    return smp_context_send_message(ctx, msg);
}

/**
 * \ingroup context
 * Process incoming data on the serial file descriptor.
 * New message will be posted to the dedicated callback
 *
 * @param[in] ctx the TupContext
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_context_process_fd(TupContext *ctx)
{
    return smp_context_process_fd(ctx);
}

/**
 * \ingroup context
 * Wait and process event data.
 *
 * @param[in] ctx the TupContext
 * @param[in] timeout_ms a timeout in milliseconds. A negative value means no
 *                       timeout
 *
 * @return 0 on success, a SmpError otherwise.
 */
int tup_context_wait_and_process(TupContext *ctx, int timeout_ms)
{
    return smp_context_wait_and_process(ctx, timeout_ms);
}
