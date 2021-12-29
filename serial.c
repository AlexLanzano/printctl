#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <error.h>
#include <termios.h>

error_t serial_init(const char *serial_device_path, const uint64_t baud, int *serial_fd)
{
    if (!serial_device_path || !serial_fd) {
        errno = EINVAL;
        return ERROR;
    }

    int baud_rate;
    *serial_fd = open(serial_device_path, O_RDWR | O_NOCTTY | O_SYNC);
    if (*serial_fd == -1) {
        return ERROR;
    }

    switch (baud) {
    case 115200:
        baud_rate = B115200;
    }

    struct termios serial_device;
    int error = tcgetattr(*serial_fd, &serial_device);
    if (error) {
        return ERROR;
    }

    cfsetospeed(&serial_device, baud_rate);
    cfsetispeed(&serial_device, baud_rate);

    serial_device.c_iflag &= ~(IGNBRK | BRKINT | ICRNL |
                               INLCR | PARMRK | INPCK | ISTRIP | IXON);
    serial_device.c_oflag = 0;
    serial_device.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);

    serial_device.c_cflag &= ~(CSIZE | PARENB);
    serial_device.c_cflag |= CS8;

    serial_device.c_cc[VMIN]  = 1;
    serial_device.c_cc[VTIME] = 0;

    error = tcsetattr(*serial_fd, TCSAFLUSH, &serial_device);
    if (error) {
        return ERROR;
    }

    return SUCCESS;
}

error_t serial_read(const int serial_fd, char *buffer, const size_t buffer_size)
{
    return SUCCESS;
}

error_t serial_write(const int serial_fd, char *data)
{
    if (write(serial_fd, data, strlen(data)) == -1) {
        return ERROR;
    }

    return SUCCESS;
}
