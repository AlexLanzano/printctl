#ifndef DAEMON_H
#define DAEMON_H

#include <common/error.h>
#include <common/socket.h>
#include <daemon/profile.h>

error_t daemon_init(profile_t profile);
error_t daemon_deinit(void);
error_t daemon_wait_for_client_connection(void);
error_t daemon_disconnect_client(void);
error_t daemon_read_client_msg(char *client_msg, size_t client_msg_length);
error_t daemon_write_msg_to_client(char *client_msg, size_t client_msg_length);
error_t daemon_reconnect_printer(const profile_t profile);

#endif
