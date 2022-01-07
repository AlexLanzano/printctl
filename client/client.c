#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <common/error.h>
#include <common/socket.h>

static socket_handle_t g_socket = 0;
static bool g_is_connected = false;

error_t client_connect(const char *profile)
{
    if (g_is_connected) {
        // TODO: Change this to better error
        return EINVAL;
    }

    error_t error;

    error = socket_connect(profile, &g_socket);
    if (error) {
        return error;
    }

    g_is_connected = true;
    return SUCCESS;
}

error_t client_disconnect()
{
    if (!g_is_connected) {
        return SUCCESS;
    }

    socket_close(g_socket);

    g_is_connected = false;
    return SUCCESS;
}

error_t client_print(const char *file_path)
{
    if (!g_is_connected) {
        return EINVAL;
    }

    char msg[1024] = {0};
    snprintf(msg, 1024, "print %s", file_path);

    return socket_write(g_socket, msg, strnlen(msg, 1024));
}

error_t client_reconnect_printer()
{
    if (!g_is_connected) {
        return EINVAL;
    }

    return socket_write(g_socket, "reconnect", 9);

}
