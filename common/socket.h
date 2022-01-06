#ifndef SOCKET_H
#define SOCKET_H

#include <stddef.h>
#include <common/error.h>

typedef int socket_handle_t;

error_t socket_connect(const char *name, socket_handle_t *handle);
error_t socket_close(socket_handle_t handle);

error_t socket_bind(const char *name, socket_handle_t *handle);
error_t socket_unbind(const char *name, socket_handle_t handle);

error_t socket_listen(socket_handle_t handle);
error_t socket_accept(socket_handle_t handle, socket_handle_t *new_connection_handle);

error_t socket_read(socket_handle_t handle, char *buffer, size_t buffer_length, size_t *bytes_read);
error_t socket_write(socket_handle_t handle, char *buffer, size_t buffer_length);

#endif
