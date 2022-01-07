#include <stdbool.h>
#include <string.h>
#include <common/error.h>
#include <common/socket.h>
#include <daemon/daemon.h>
#include <daemon/profile.h>
#include <daemon/printer.h>

static socket_handle_t g_daemon_socket;
static bool g_daemon_is_connected = false;
static char g_daemon_socket_name[108] = {0};

static socket_handle_t g_client_socket;
static bool g_client_is_connected = false;

error_t daemon_init(profile_t profile)
{
    error_t error;

    strncpy(g_daemon_socket_name, profile.name, 108);

    error = printer_connect(profile.serial_device, profile.baud);
    if (error) {
        print_error("Failed to connect to printer.");
        return error;
    }

    error = socket_bind(profile.name, &g_daemon_socket);
    if (error) {
        print_error("Failed to bind to the socket.");
        return error;
    }

    error = socket_listen(g_daemon_socket);
    if (error) {
        print_error("Failed to listen on socket.");
        return error;
    }

    g_daemon_is_connected = true;

    return SUCCESS;
}

error_t daemon_deinit()
{
    error_t error;

    error = daemon_disconnect_client();
    if (error) {
        print_error("Failed to close client socket");
    }

    if (!g_daemon_is_connected) {
        return SUCCESS;
    }

    error = socket_unbind(g_daemon_socket_name, g_daemon_socket);
    if (error) {
        print_error("Failed to close daemon socket.");
        return error;
    }

    return SUCCESS;
}

error_t daemon_wait_for_client_connection()
{
    error_t error;

    error = socket_accept(g_daemon_socket, &g_client_socket);
    if (error) {
        print_error("Fail to create connection to client.");
        return error;
    }

    g_client_is_connected = true;
    return SUCCESS;
}

error_t daemon_disconnect_client()
{
    if (!g_client_is_connected) {
        return SUCCESS;
    }

    g_client_is_connected = false;
    return socket_close(g_client_socket);
}

error_t daemon_read_client_msg(char *client_msg, size_t client_msg_length)
{
    if (!g_client_is_connected) {
        print_error("Client is not connected.");
        return ECOMM;
    }

    size_t bytes_read;
    return socket_read(g_client_socket, client_msg, client_msg_length, &bytes_read);
}

error_t daemon_write_msg_to_client(char *client_msg, size_t client_msg_length)
{
    if (!g_client_is_connected) {
        print_error("Client is not connected.");
        return ECOMM;
    }

    return socket_write(g_client_socket, client_msg, client_msg_length);
}

error_t daemon_reconnect_printer(const profile_t profile)
{
    error_t error;

    error = printer_disconnect();
    if (error) {
        print_error("Failed to disconnect printer.");
        return error;
    }

    error = printer_connect(profile.serial_device, profile.baud);
    if (error) {
        print_error("Failed to connect to printer.");
        return error;
    }

    return SUCCESS;
}
