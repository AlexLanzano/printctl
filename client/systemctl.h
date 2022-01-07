#ifndef SYSTEMCTL_H
#define SYSTEMCTL_H

#include <common/error.h>

error_t systemctl_start(const char *profile);
error_t systemctl_stop(const char *profile);
error_t systemctl_enable(const char *profile);
error_t systemctl_disable(const char *profile);
error_t systemctl_status(const char *profile);

#endif
