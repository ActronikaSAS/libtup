/* libtup
 * Copyright (C) 2017 Actronika SAS
 *     Author: Aurélien Zanelli <aurelien.zanelli@actronika.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libtup.h>

#define N_ELEMENTS(arr) (sizeof((arr)) / sizeof((arr)[0]))

#define TIME_FORMAT "u:%02u:%02u.%03u"
#define TIME_ARGS(time_us) \
    (unsigned int) (time_us / 1000 / 1000 / 3600), \
    (unsigned int) (time_us / 1000 / 1000 % 3600 / 60), \
    (unsigned int) (time_us / 1000 / 1000 % 60), \
    (unsigned int) (time_us / 1000 % 1000)

static void print_help_and_exit(void);

/* Command handling */
typedef struct
{
    const char *name;
    const char *args_desc;
    const char *desc;
    int (*callback)(int argc, char *argv[]);
} Command;

/* Options parsing as getopt is not available on Windows (rewrite our own) */
typedef struct {
    const char short_name;
    const char *long_name;
    const char *description;
    int *flag;
    void (*action)(void);
} Option;

static void do_cmdline_option(const Option *opt)
{
    if (opt->flag != NULL)
        *(opt->flag) = 1;

    if (opt->action != NULL)
        opt->action();
}

static int parse_cmdline_options(const Option *options, size_t n_options,
        int *argc, char **argv[])
{
    int i;
    int rem_argc = 0;

    for (i = 0; i < *argc; i++) {
        char *arg = (*argv)[i];
        int remove = 0;
        size_t j;

        for (j = 0; j < n_options; j++) {
            const Option *opt = &options[j];
            char opt_short[3] = { 0, };
            char opt_long[32] = { 0, };

            snprintf(opt_short, sizeof(opt_short) - 1, "-%c", opt->short_name);
            snprintf(opt_long, sizeof(opt_long) - 1, "--%s", opt->long_name);

            if (strcmp(arg, opt_short) == 0 || strcmp(arg, opt_long) == 0) {
                do_cmdline_option(opt);
                remove = 1;
                break;
            }
        }

        if (!remove) {
            (*argv)[rem_argc] = arg;
            rem_argc++;
        }
    }

    *argc = rem_argc;

    return 0;
}

static void print_cmdline_options(const Option *options, size_t n_options)
{
    size_t i;

    for (i = 0; i < n_options; i++) {
        printf("  -%c, --%-30s %s\n", options[i].short_name,
                options[i].long_name, options[i].description);
    }
}

/* some globals variables */
static TupContext *tup_ctx;
static int response_recv;

static void sort_task_by_id(TupDebugTaskStatus *in, TupDebugTaskStatus *out,
        size_t n_tasks)
{
    size_t i, j;

    for (i = 0; i < n_tasks; i++) {
        uint32_t lid = UINT32_MAX;
        TupDebugTaskStatus *cur = NULL;

        /* find the lowest id in `in` */
        for (j = 0; j < n_tasks; j++) {
            if (in[j].id < lid) {
                lid = in[j].id;
                cur = in + j;
            }
        }

        if (cur == NULL)
            break;

        out[i] = *cur;
        cur->id = UINT32_MAX;
    }
}

static void handle_debug_system_status_response(SmpMessage *message)
{
    TupDebugSystemStatus status;
    TupDebugTaskStatus unsorted_tasks[16];
    TupDebugTaskStatus tasks[16];
    int n_tasks;
    struct {
        unsigned int running;
        unsigned int ready;
        unsigned int waiting;
        unsigned int stopped;
    } tasks_stats = { 0, 0, 0, 0};
    int i;

    n_tasks = tup_message_init_parse_debug_system_status(message, &status,
            unsorted_tasks, N_ELEMENTS(unsorted_tasks));
    if (n_tasks < 0) {
        fprintf(stderr, "failed to parse system status response\n");
        return;
    }

    sort_task_by_id(unsorted_tasks, tasks, n_tasks);

    for (i = 0; i < n_tasks; i++) {
        switch (tasks[i].state) {
            case TUP_DEBUG_TASK_STATE_READY:
                tasks_stats.ready++;
                break;
            case TUP_DEBUG_TASK_STATE_RUNNING:
                tasks_stats.running++;
                break;
            case TUP_DEBUG_TASK_STATE_BLOCKED:
                tasks_stats.waiting++;
                break;
            case TUP_DEBUG_TASK_STATE_SUSPENDED:
            case TUP_DEBUG_TASK_STATE_DELETED:
            default:
                tasks_stats.stopped++;
                break;
        }
    }

    printf("Uptime: %02u:%02u:%02u\n",
            (unsigned int)(status.rtime / 1000 / 1000 / 3600),
            (unsigned int)(status.rtime / 1000 / 1000 % 3600 / 60),
            (unsigned int)(status.rtime / 1000 / 1000 % 60));
    printf("Tasks: %u total, %u running, %u ready, %u waiting, %u stopped\n",
            n_tasks, tasks_stats.running, tasks_stats.ready,
            tasks_stats.waiting, tasks_stats.stopped);
    printf("Mem (B): %u total, %u used, %u free\n\n", status.mem_total,
            status.mem_used, status.mem_total - status.mem_used);

    printf("TID  ST  PR  RemStk  Time           name\n");
    for (i = 0; i < n_tasks; i++) {
        char c;

        switch (tasks[i].state) {
            case TUP_DEBUG_TASK_STATE_READY:
                c = 'R';
                break;
            case TUP_DEBUG_TASK_STATE_RUNNING:
                c = 'r';
                break;
            case TUP_DEBUG_TASK_STATE_BLOCKED:
                c = 'B';
                break;
            case TUP_DEBUG_TASK_STATE_SUSPENDED:
                c = 'S';
                break;
            case TUP_DEBUG_TASK_STATE_DELETED:
                c = 'D';
                break;
            case TUP_DEBUG_TASK_STATE_NONE:
            default:
                c = 'U';
                break;
        }

        printf("%3u  %-2c  %2u  %6u  %" TIME_FORMAT "    %s\n",
                tasks[i].id, c, tasks[i].priority, tasks[i].rem_stack,
                TIME_ARGS(tasks[i].time), tasks[i].name);
    }
}

/* RX message handling */
static void on_tup_message(TupContext *ctx, TupMessage *message, void *userdata)
{
    switch (TUP_MESSAGE_TYPE(message)) {
        case TUP_MESSAGE_ACK:
            printf("done\n");
            break;
        case TUP_MESSAGE_ERROR: {
            TupMessageType cmd;
            uint32_t error;

            tup_message_parse_error(message, &cmd, &error);
            printf("error: 0x%08x\n", error);
            break;
        }
        case TUP_MESSAGE_RESP_VERSION: {
            const char *version;

            tup_message_parse_resp_version(message, &version);
            printf("tactronik version: %s\n", version);
            break;
        }
        case TUP_MESSAGE_RESP_PARAMETER: {
            uint8_t id;
            TupParameterArgs args[1];

            tup_message_parse_resp_parameter(message, &id, args,
                    N_ELEMENTS(args));
            printf("effect %d parameter %d value is %d\n", id,
                    args[0].parameter_id, args[0].parameter_value);
            break;
        }
        case TUP_MESSAGE_RESP_SENSOR: {
            TupSensorValueArgs args[1];

            tup_message_parse_resp_sensor(message, args, N_ELEMENTS(args));
            printf("sensor %d value is %d\n", args[0].sensor_id,
                    args[0].sensor_value);
            break;
        }
        case TUP_MESSAGE_RESP_INPUT: {
            uint8_t effect_slot_id;
            TupInputValueArgs args[1];

            tup_message_parse_resp_input(message, &effect_slot_id, args,
                    N_ELEMENTS(args));
            printf("input %d of effect %d have value %d\n", args[0].input_id,
                    effect_slot_id, args[0].input_value);
            break;
        }
        case TUP_MESSAGE_RESP_BUILDINFO: {
            const char *buildinfo;

            tup_message_parse_resp_buildinfo(message, &buildinfo);
            printf("build information:\n%s", buildinfo);
            break;
        }
        case TUP_MESSAGE_RESP_DEBUG_SYSTEM_STATUS:
            handle_debug_system_status_response(message);
            break;
        default:
            printf("Unhandled message id %d\n", TUP_MESSAGE_TYPE(message));
            break;
    }

    response_recv = 1;
}

static void on_tup_error(TupContext *ctx, SmpError error, void *userdata)
{
    fprintf(stderr, "Tup error: %d", error);
}

/* commands */
static int do_load(int argc, char *argv[])
{
    TupMessage *msg;
    int loaded_id;
    int effect_id;
    int ret;

    if (argc != 2) {
        printf("'load' arguments: <slot-id> <effect-id>\n");
        return -EINVAL;
    }

    ret = sscanf(argv[0], "%d", &loaded_id);
    ret += sscanf(argv[1], "%d", &effect_id);
    if (ret != 2) {
        printf("failed to parse arguments\n");
        return -EINVAL;
    }

    printf("Loading effect %d to slot %d\n", effect_id, loaded_id);
    msg = tup_message_new();
    tup_message_init_load(msg, loaded_id, effect_id);
    ret = tup_context_send(tup_ctx, msg);
    tup_message_free(msg);
    return ret;
}

static int do_play(int argc, char *argv[])
{
    TupMessage *msg;
    int effect_id;
    int ret;

    if (argc != 1) {
        printf("'play' arguments: <slot-id>\n");
        return -EINVAL;
    }

    ret = sscanf(argv[0], "%d", &effect_id);
    if (ret != 1) {
        printf("failed to parse arguments\n");
        return -EINVAL;
    }

    printf("Playing effect in slot %d\n", effect_id);
    msg = tup_message_new();
    tup_message_init_play(msg, effect_id);
    ret = tup_context_send(tup_ctx, msg);
    tup_message_free(msg);
    return ret;
}

static int do_stop(int argc, char *argv[])
{
    TupMessage *msg;
    int effect_id;
    int ret;

    if (argc != 1) {
        printf("'stop' arguments: <slot-id>\n");
        return -EINVAL;
    }

    ret = sscanf(argv[0], "%d", &effect_id);
    if (ret != 1) {
        printf("failed to parse arguments\n");
        return -EINVAL;
    }

    printf("Stopping effect in slot %d\n", effect_id);
    msg = tup_message_new();
    tup_message_init_stop(msg, effect_id);
    ret = tup_context_send(tup_ctx, msg);
    tup_message_free(msg);
    return ret;
}

static int do_get_version(int argc, char *argv[])
{
    TupMessage *msg;
    int ret;

    printf("Getting version\n");
    msg = tup_message_new();
    tup_message_init_get_version(msg);
    ret = tup_context_send(tup_ctx, msg);
    tup_message_free(msg);
    return ret;
}

static int do_get_buildinfo(int argc, char *argv[])
{
    TupMessage *msg;
    int ret;

    printf("Getting buildinfo\n");
    msg = tup_message_new();
    tup_message_init_get_buildinfo(msg);
    ret = tup_context_send(tup_ctx, msg);
    tup_message_free(msg);
    return ret;
}

static int do_get_parameter(int argc, char *argv[])
{
    TupMessage *msg;
    int effect_id, parameter_id;
    int ret;

    if (argc != 2) {
        printf("'get_parameter' arguments: <slot-id> <parameter-id>\n");
        return -EINVAL;
    }

    ret = sscanf(argv[0], "%d", &effect_id);
    ret += sscanf(argv[1], "%d", &parameter_id);
    if (ret != 2) {
        printf("failed to parse arguments\n");
        return -EINVAL;
    }

    printf("Getting effect %d parameter %d\n", effect_id, parameter_id);
    msg = tup_message_new();
    tup_message_init_get_parameter_simple(msg, effect_id, parameter_id);
    ret = tup_context_send(tup_ctx, msg);
    tup_message_free(msg);
    return ret;
}

static int do_set_parameter(int argc, char *argv[])
{
    TupMessage *msg;
    int effect_id, parameter_id, parameter_value;
    int ret;

    if (argc != 3) {
        printf("'set_parameter' arguments: <slot-id> <parameter-id> <value>\n");
        return -EINVAL;
    }

    ret = sscanf(argv[0], "%d", &effect_id);
    ret += sscanf(argv[1], "%d", &parameter_id);
    ret += sscanf(argv[2], "%d", &parameter_value);
    if (ret != 3) {
        printf("failed to parse arguments\n");
        return -EINVAL;
    }

    printf("Setting effect %d parameter %d to %d\n", effect_id, parameter_id,
            parameter_value);
    msg = tup_message_new();
    tup_message_init_set_parameter_simple(msg, effect_id, parameter_id,
            parameter_value);
    ret = tup_context_send(tup_ctx, msg);
    tup_message_free(msg);
    return ret;
}

static int do_bind_effect(int argc, char *argv[])
{
    TupMessage *msg;
    int effect_id, flags;
    int ret;

    if (argc != 2) {
        printf("'bind_effect' arguments: <slot-id> <binding-flags>\n"
                "binding-flags: 0 -> unbind\n"
                "               1 -> actuator 1\n"
                "               2 -> actuator 2\n"
                "               3 -> actuator 1 and 2\n");
        return -EINVAL;
    }

    ret = sscanf(argv[0], "%d", &effect_id);
    ret += sscanf(argv[1], "%d", &flags);
    if (ret != 2) {
        printf("failed to parse arguments\n");
        return -EINVAL;
    }

    printf("Binding effect %d to actuators %s\n", effect_id,
            (flags == 3) ? "1 and 2" :
            (flags & 0x1) ? "1" :
            (flags & 0x2) ? "2" : "0");
    msg = tup_message_new();
    tup_message_init_bind_effect(msg, effect_id, flags);
    ret = tup_context_send(tup_ctx, msg);
    tup_message_free(msg);
    return ret;
}

static int do_get_sensor_value(int argc, char *argv[])
{
    TupMessage *msg;
    int sensor_id;
    int ret;

    if (argc != 1) {
        printf("'get_sensor_value' arguments: <sensor-id>\n");
        return -EINVAL;
    }

    ret = sscanf(argv[0], "%d", &sensor_id);
    if (ret != 1) {
        printf("failed to parse arguments\n");
        return -EINVAL;
    }

    msg = tup_message_new();
    tup_message_init_get_sensor_value_simple(msg, sensor_id);
    ret = tup_context_send(tup_ctx, msg);
    tup_message_free(msg);
    return ret;
}

static int do_set_sensor_value(int argc, char *argv[])
{
    TupMessage *msg;
    int sensor_id;
    int sensor_value;
    int ret;

    if (argc != 2) {
        printf("'set_sensor_value' arguments: <sensor-id> <value>\n");
        return -EINVAL;
    }

    ret = sscanf(argv[0], "%d", &sensor_id);
    ret += sscanf(argv[1], "%d", &sensor_value);
    if (ret != 2) {
        printf("failed to parse arguments\n");
        return -EINVAL;
    }

    msg = tup_message_new();
    tup_message_init_set_sensor_value_simple(msg, sensor_id, sensor_value);
    ret = tup_context_send(tup_ctx, msg);
    tup_message_free(msg);
    return ret;
}

static int do_get_input_value(int argc, char *argv[])
{
    TupMessage *msg;
    int effect_slot_id;
    int input_id;
    int ret;

    if (argc != 2) {
        printf("'get_input_value' arguments: <effect slot id> <input-id>\n");
        return -EINVAL;
    }

    ret = sscanf(argv[0], "%d", &effect_slot_id);
    ret += sscanf(argv[1], "%d", &input_id);
    if (ret != 2) {
        printf("failed to parse arguments\n");
        return -EINVAL;
    }
    printf("Slot : %d\ninput : %d\n", effect_slot_id, input_id);

    msg = tup_message_new();
    tup_message_init_get_input_value_simple(msg, effect_slot_id, input_id);
    ret = tup_context_send(tup_ctx, msg);
    tup_message_free(msg);
    return ret;
}

static int do_set_input_value(int argc, char *argv[])
{
    TupMessage *msg;
    int effect_slot_id;
    int input_id;
    int input_value;
    int ret;

    if (argc != 3) {
        printf("'set_input_value' arguments: <effect slot id> <input-id> <value>\n");
        return -EINVAL;
    }


    ret = sscanf(argv[0], "%d", &effect_slot_id);
    ret += sscanf(argv[1], "%d", &input_id);
    ret += sscanf(argv[2], "%d", &input_value);
    if (ret != 3) {
        printf("failed to parse arguments\n");
        return -EINVAL;
    }
    printf("Slot : %d\ninput : %d\nvalue : %d\n", effect_slot_id, input_id, input_value);

    msg = tup_message_new();
    tup_message_init_set_input_value_simple(msg, effect_slot_id, input_id,
            input_value);
    ret = tup_context_send(tup_ctx, msg);
    tup_message_free(msg);
    return ret;
}

static int do_activate_internal_sensors(int argc, char *argv[])
{
    TupMessage *msg;
    int state_activation;
    int ret;

    if (argc != 1) {
        printf("'activate_internal_sensors' arguments : <state active>");
        return -EINVAL;
    }

    ret = sscanf(argv[0], "%d", &state_activation);
    if (ret != 1) {
        printf("failed to parse arguments\n");
        return -EINVAL;
    }
    switch(state_activation) {
        case 0:
            printf("deactivate internal sensors \n");
            break;
        case 1:
            printf("activate internal sensors\n");
            break;
        default:
            printf("Internal Sensors : Unknown value\n");
            goto error;
            break;
    }
    msg = tup_message_new();
    tup_message_init_activate_internal_sensors(msg, state_activation);
    ret = tup_context_send(tup_ctx, msg);
    tup_message_free(msg);
    return ret;

error:
    return -EINVAL;
}

static int do_debug_get_system_status(int argc, char *argv[])
{
    TupMessage *msg;
    int ret;

    msg = tup_message_new();
    tup_message_init_cmd_debug_get_system_status(msg);
    ret = tup_context_send(tup_ctx, msg);
    tup_message_free(msg);

    return ret;
}

static const Command cmds[] = {
    { "load", "<slot-id> <effect-id>", "load given effect in slot", do_load },
    { "play", "<slot-id>", "play the effect in given slot", do_play },
    { "stop", "<slot-id>", "stop the effect in given slot", do_stop },
    { "get_version", "", "get the tactronik version", do_get_version },
    { "get_buildinfo", "", "get the tactronik build info", do_get_buildinfo },
    { "get_parameter", "<slot-id> <parameter-id>",
        "get parameter with givent id of effect in given slot",
        do_get_parameter },
    { "set_parameter", "<slot-id> <parameter-id> <value>",
        "set value of parameter of effect in given slot", do_set_parameter },
    { "bind_effect", "<slot-id> <binding-flags>",
        "bind effect in given slot with actuators (0: unbind)",
        do_bind_effect },
    { "get_sensor_value", "<sensor-id>", "get the value of the given sensor",
        do_get_sensor_value },
    { "set_sensor_value", "<sensor-id> <value>",
        "set the value of the given sensor", do_set_sensor_value },
    { "get_input_value", "<effect slot id> <input-id>",
        "get the value of the given input", do_get_input_value },
    { "set_input_value", "<effect slot id> <input-id> <value>",
        "set the value of the given input", do_set_input_value },
    { "activate_sensors", "<state>",
        "Activate (1) or not (0) the management of internal sensors",
        do_activate_internal_sensors},
    { "get_sys_status", "", "Get the system status (debug)",
        do_debug_get_system_status},
};

static const Option options[] = {
    { 'h', "help", "Show this help", NULL, print_help_and_exit },
};

static void print_help_and_exit(void)
{
    size_t i;

    printf("Available commands:\n");
    for (i = 0; i < N_ELEMENTS(cmds); i++) {
        printf("  %-16s %-40s%s\n", cmds[i].name, cmds[i].args_desc,
                cmds[i].desc);
    }

    printf("\nOptions:\n");
    print_cmdline_options(options, N_ELEMENTS(options));
    exit(0);
}

static void usage(const char *pname)
{
    printf("Usage: %s [--help] <device> <cmd> [args]\n", pname);
}

int main(int argc, char *argv[])
{
    TupCallbacks cbs = {
        .new_message_cb = on_tup_message,
        .error_cb = on_tup_error,
    };
    const char *device;
    const char *cmd;
    int ret;
    size_t i;

    parse_cmdline_options(options, N_ELEMENTS(options), &argc, &argv);

    if (argc < 3) {
        usage(argv[0]);
        return 1;
    }

    device = argv[1];
    cmd = argv[2];

    tup_ctx = tup_context_new(&cbs, NULL);
    if (tup_ctx == NULL) {
        fprintf(stderr, "failed to create a tup context\n");
        return 1;
    }

    ret = tup_context_open(tup_ctx, device);
    if (ret < 0) {
        fprintf(stderr, "error while initializing tup context: %d", ret);
        return 1;
    }

    /* check command and call its callback */
    for (i = 0; i < N_ELEMENTS(cmds); i++) {
        if (strcmp(cmds[i].name, cmd) == 0) {
            ret = cmds[i].callback(argc - 3, argv + 3);
            break;
        }
    }

    if (i == N_ELEMENTS(cmds)) {
        fprintf(stderr, "command not found\n");
        ret = 1;
        goto done;
    }

    if (ret < 0)
        goto done;

    /* wait for a response message */
    response_recv = 0;
    while (!response_recv) {
        ret = tup_context_wait_and_process(tup_ctx, 2000);
        if (ret < 0) {
            if (ret == SMP_ERROR_TIMEDOUT)
                printf("timeout while waiting for response\n");
            else
                printf("an error occurs while waiting for response\n");

            goto done;
        }
    }

done:
    tup_context_free(tup_ctx);

    return ret;
}
