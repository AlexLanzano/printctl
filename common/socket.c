#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <common/socket.h>


error_t socket_connect(const char *name, socket_handle_t *handle)
{
    if (!name || !handle) {
        return EINVAL;
    }

    error_t error;
    struct sockaddr_un sockaddr;

    *handle = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (*handle < 0) {
        return errno;
    }

    sockaddr.sun_family = AF_LOCAL;
    snprintf(sockaddr.sun_path, 108, "/tmp/%s", name);

    error = connect(*handle, (const struct sockaddr *)&sockaddr, sizeof(sockaddr));
    if (error) {
        close(*handle);
        return errno;
    }

    return SUCCESS;
}

error_t socket_close(socket_handle_t handle)
{
    if (close(handle) == -1) {
        return errno;
    }
    return SUCCESS;
}

error_t socket_bind(const char *name, socket_handle_t *handle)
{
    if (!name || !handle) {
        return EINVAL;
    }

    error_t error;
    struct sockaddr_un sockaddr;

    *handle = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (*handle < 0) {
        return errno;
    }

    sockaddr.sun_family = AF_LOCAL;
    snprintf(sockaddr.sun_path, 108, "/tmp/%s", name);

    error = bind(*handle, (const struct sockaddr *)&sockaddr, sizeof(sockaddr));
    if (error) {
        close(*handle);
        return errno;
    }
    return SUCCESS;
}

error_t socket_unbind(const char *name, socket_handle_t handle)
{
    if (!name) {
        return EINVAL;
    }

    error_t error;
    char socket_name[108] = {0};

    error = socket_close(handle);
    if (error) {
        return error;
    }

    snprintf(socket_name, 108, "/tmp/%s", name);
    if (unlink(socket_name) == -1) {
        return errno;
    }

    return SUCCESS;
}

error_t socket_listen(socket_handle_t handle)
{
    if (listen(handle, 20) == -1) {
        return errno;
    }
    return SUCCESS;

}

error_t socket_accept(socket_handle_t handle, socket_handle_t *new_connection_handle)
{
    *new_connection_handle = accept(handle, NULL, NULL);
    if (*new_connection_handle == -1) {
        return errno;
    }
    return SUCCESS;
}

error_t socket_read(socket_handle_t handle, char *buffer, size_t buffer_length, size_t *bytes_read)
{
    if ((*bytes_read = read(handle, buffer, buffer_length)) == -1) {
        return errno;
    }
    return SUCCESS;
}

error_t socket_write(socket_handle_t handle, char *buffer, size_t buffer_length)
{
    if (write(handle, buffer, buffer_length) == -1) {
        return errno;
    }
    return SUCCESS;

}
