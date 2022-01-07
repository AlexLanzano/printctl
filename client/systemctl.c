#include <stdio.h>
#include <unistd.h>
#include <client/systemctl.h>
#include <common/error.h>

error_t systemctl_start(const char *profile)
{
    if (!profile) {
        return EINVAL;
    }

    char service[1024] = {0};
    snprintf(service, 1024, "printctl@%s", profile);

    execl("/usr/bin/systemctl", "systemctl", "start", service, NULL);
    return SUCCESS;
}

error_t systemctl_stop(const char *profile)
{
    if (!profile) {
        return EINVAL;
    }

    char service[1024] = {0};
    snprintf(service, 1024, "printctl@%s", profile);

    execl("/usr/bin/systemctl", "systemctl", "stop", service, NULL);
    return SUCCESS;

}

error_t systemctl_enable(const char *profile)
{
    if (!profile) {
        return EINVAL;
    }

    char service[1024] = {0};
    snprintf(service, 1024, "printctl@%s", profile);

    execl("/usr/bin/systemctl", "systemctl", "enable", service, NULL);
    return SUCCESS;
}

error_t systemctl_disable(const char *profile)
{
    if (!profile) {
        return EINVAL;
    }

    char service[1024] = {0};
    snprintf(service, 1024, "printctl@%s", profile);

    execl("/usr/bin/systemctl", "systemctl", "start", service, NULL);
    return SUCCESS;
}

error_t systemctl_status(const char *profile)
{
    if (!profile) {
        return EINVAL;
    }

    char service[1024] = {0};
    snprintf(service, 1024, "printctl@%s", profile);

    execl("/usr/bin/systemctl", "systemctl", "status", service, NULL);
    return SUCCESS;
}
