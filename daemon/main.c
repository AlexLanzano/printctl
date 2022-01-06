#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <common/error.h>
#include <daemon/profile.h>
#include <daemon/daemon.h>
#include <daemon/printer.h>


#define INPUT_LENGTH 256
#define COMMAND_STRING_MAX 32
#define COMMAND_COUNT 1

typedef enum command {
    COMMAND_INVALID,
    COMMAND_PRINT,
} command_t;

static const char g_command[COMMAND_COUNT][COMMAND_STRING_MAX] = {{"print"}};

static void print_help()
{
    printf("Usage: printctld [PROFILE]\n");
}

static void interrupt_signal_handler(int signal)
{
    daemon_deinit();
    exit(0);
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

static error_t process_print_command()
{
    char *filename = strtok(NULL, " ");
    if (!filename) {
        return EINVAL;
    }

    return printer_print(filename);
}

static error_t process_client_msg(char *msg)
{
    if (!msg) {
        return EINVAL;
    }

    error_t error;
    char *command = strtok(msg, " ");
    if (!command) {
        return EINVAL;
    }

    switch (parse_command(command)) {
    case COMMAND_INVALID:
        print_error("Invalid command.");
        error = EINVAL;
        break;

    case COMMAND_PRINT:
        error = process_print_command();
        break;
    }

    return error;
}

int32_t main(int32_t argc, char **argv)
{
    if (argc < 2) {
        print_help();
        return EINVAL;
    }

    error_t error;
    profile_t profile;

    if (signal(SIGINT, interrupt_signal_handler) == SIG_ERR) {
        print_error("Unable to catch SIGINT signal.");
        return EINVAL;
    }

    error = profile_init(argv[1], &profile);
    if (error) {
        print_error("Unable to process profile.");
        return error;
    }

    error = daemon_init(profile);
    if (error) {
        print_error("Unable to initialize daemon.");
        return error;
    }

    // TODO: Implement some signal here
    while (1) {
        char client_msg[1024] = {0};

        daemon_wait_for_client_connection();
        daemon_read_client_msg(client_msg, 1024);

        process_client_msg(client_msg);

        daemon_disconnect_client();
    }

    error = daemon_deinit();
    if (error) {
        print_error("Unable to deinitialize daemon.");
        return error;
    }

    return 0;
}
