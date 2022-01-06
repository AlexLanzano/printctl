#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <common/error.h>
#include <daemon/profile.h>

static error_t profile_clean_line(char *line)
{
    if (!line) {
        return EINVAL;
    }

    if (line[0] == '#') {
        line[0] = 0;
    }

    size_t line_length = strlen(line);
    for (size_t i = 0; i < line_length; i++) {
        if (line[i] == '#' || line[i] == '\n') {
            line[i] = 0;
            break;
        }
    }

    return SUCCESS;
}

static error_t profile_parse(FILE *file, profile_t *profile)
{
    if (!file || !profile) {
        return EINVAL;
    }

    char line[1024] = {0};

    while (fgets(line, 1024, file)) {
        if (line[0] == 0 ||
            line[0] == '\n') {
            continue;
        }

        profile_clean_line(line);

        char *token = strtok(line, "=");
        if (strncmp(line, "device", 6) == 0) {
            token = strtok(NULL, "=");
            strncpy(profile->serial_device, token, 32);

        } else if (strncmp(line, "baud", 6) == 0) {
            char baud[32] = {0};
            token = strtok(NULL, "");
            strncpy(baud, token, 32);
            profile->baud = atoi(baud);

        } else {
            print_error("Invalid profile.");
            return EINVAL;
        }
    }

    return SUCCESS;
}

error_t profile_init(const char *name, profile_t *profile)
{
    if (!name || !profile) {
        return EINVAL;
    }

    strncpy(profile->name, name, 32);

    char filename[256] = {0};
    snprintf(filename, 256, "/etc/printctl/%s", name);

    printf("%s\n", filename);
    FILE *file = fopen(filename, "r");
    if (!file) {
        print_error("Unable to open file");
        return errno;
    }

    return profile_parse(file, profile);
}
