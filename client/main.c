#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <common/error.h>
#include <client/systemctl.h>
#include <client/client.h>


#define INPUT_LENGTH 256
#define COMMAND_STRING_MAX 32
#define COMMAND_COUNT 6

typedef enum command {
    COMMAND_INVALID,
    COMMAND_HELP,
    COMMAND_START,
    COMMAND_STOP,
    COMMAND_ENABLE,
    COMMAND_DISABLE,
    COMMAND_PRINT,
} command_t;

static const char g_command[COMMAND_COUNT][COMMAND_STRING_MAX] = {{"--help"},
                                                                  {"start"}, {"stop"}, {"enable"}, {"disable"},
                                                                  {"print"}};

static void print_help()
{
    printf("Usage: printctl {COMMAND} [ARGS]\n"
           "                [--help]\n\n"
           "Commands:\n"
           "  start [PROFILE] - start daemon\n"
           "  stop [PROFILE] - stop daemon\n"
           "  enable [PROFILE] - enable daemon to start on boot\n"
           "  disable [PROFILE] - disable daemon from running on boot\n"
           "  print [PROFILE] [FILE] - Tell daemon to print a file\n");
}

static command_t parse_command(const char *command)
{
    // TODO: Optimize
    for (size_t i = 0; i< COMMAND_COUNT; i++) {
        if (strncmp(command, g_command[i], strlen(g_command[i])) == 0) {
            return (command_t)i+1;
        }
    }

    return COMMAND_INVALID;
}

static error_t process_start_command(int32_t argc, char **argv)
{
    if (argc < 3) {
        print_error("Please provide a profile");
        print_help();
        return EINVAL;
    }

    char *profile = argv[2];
    systemctl_start(profile);

    return SUCCESS;
}

static error_t process_stop_command(int32_t argc, char **argv)
{
    if (argc < 3) {
        print_error("Please provide a profile");
        print_help();
        return EINVAL;
    }

    char *profile = argv[2];
    systemctl_stop(profile);

    return SUCCESS;
}

static error_t process_enable_command(int32_t argc, char **argv)
{
    if (argc < 3) {
        print_error("Please provide a profile");
        print_help();
        return EINVAL;
    }

    char *profile = argv[2];
    systemctl_enable(profile);

    return SUCCESS;
}

static error_t process_disable_command(int32_t argc, char **argv)
{
    if (argc < 3) {
        print_error("Please provide a profile");
        print_help();
        return EINVAL;
    }

    char *profile = argv[2];
    systemctl_disable(profile);

    return SUCCESS;
}

static error_t process_print_command(int32_t argc, char **argv)
{
    if (argc < 4) {
        print_help();
        return EINVAL;
    }

    error_t error;
    char *profile = argv[2];
    char *file = argv[3];

    error = client_connect(profile);
    if (error) {
        print_error("Client failed to connect to daemon.");
        return error;
    }

    error = client_print(file);
    if (error) {
        print_error("Client failed to send print command to daemon.");
        return error;
    }

    error = client_disconnect();
    if (error) {
        print_error("Client failed to disconnect from daemon.");
        return error;
    }

    return SUCCESS;
}

static error_t process_input(int32_t argc, char **argv)
{
    error_t error;

    switch (parse_command(argv[1])) {
    case COMMAND_INVALID:
        print_error("Invalid command.");
        error = EINVAL;
        break;

    case COMMAND_HELP:
        print_help();
        error = SUCCESS;
        break;

    case COMMAND_START:
        error = process_start_command(argc, argv);
        break;

    case COMMAND_STOP:
        error = process_stop_command(argc, argv);
        break;

    case COMMAND_ENABLE:
        error = process_enable_command(argc, argv);
        break;

    case COMMAND_DISABLE:
        error = process_disable_command(argc, argv);
        break;

    case COMMAND_PRINT:
        error = process_print_command(argc, argv);
        break;
    }

    return error;
}

int32_t main(int32_t argc, char **argv)
{

    if (argc == 1) {
        print_help();
        return EINVAL;
    }

    return process_input(argc, argv);
}
