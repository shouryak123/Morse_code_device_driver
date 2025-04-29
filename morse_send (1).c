#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <termios.h>

#include "morse_driver.h"

#define SERIAL_PORT "/dev/ttyUSB0"
#define BAUDRATE B115200

int main() {
    int fd, serial_fd;
    char buf[256];

    fd = open("/dev/morse_driver", O_RDWR);
    if (fd < 0) {
        perror("Failed to open /dev/morse_driver");
        return 1;
    }

    serial_fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY);
    if (serial_fd < 0) {
        perror("Failed to open serial port");
        close(fd);
        return 1;
    }

    struct termios options;
    tcgetattr(serial_fd, &options);
    cfsetispeed(&options, BAUDRATE);
    cfsetospeed(&options, BAUDRATE);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CRTSCTS;
    tcsetattr(serial_fd, TCSANOW, &options);

    printf("Listening on serial: %s\n", SERIAL_PORT);

    while (1) {
        memset(buf, 0, sizeof(buf));
        int n = read(serial_fd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';

            for (int i = 0; i < n; i++) {
                if (buf[i] == '\r' || buf[i] == '\n') {
                    buf[i] = '\0';
                    break;
                }
            }

            if (strlen(buf) > 0) {
                printf("Read from serial: '%s'\n", buf);

                if (ioctl(fd, MORSE_IOC_SEND, buf) < 0) {
                    perror("ioctl failed");
                } else {
                    printf("Sent to morse_driver successfully!\n");
                }
            }
        }

        usleep(100000);
    }

    close(serial_fd);
    close(fd);

    return 0;
}
