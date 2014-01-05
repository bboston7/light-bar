#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include "light_bar.h"

#define BAUD B115200
#define PATH "/dev/ttyACM0"

static const int RESET    = 0;
static const int SOLID    = 1;
static const int PROGRESS = 2;
static const int RANDOM   = 3;
static const int CUSTOM   = 4;

light_bar lb_connect() {
    int res;
    light_bar ret = open(PATH, O_RDWR | O_NOCTTY);
    assert(ret != -1);

    struct termios opts;
    res = tcgetattr(ret, &opts);
    assert(res == 0);

    // Set baud
    res = cfsetspeed(&opts, BAUD);
    assert(res == 0);

    cfmakeraw(&opts);

    res = tcsetattr(ret, TCSANOW, &opts);
    assert(res == 0);

    // Wait for arduino to become ready
    char ready = 0;
    while (res != 1) {
        res = read(ret, &ready, 1);
    }

    return ret;
}

int lb_close(light_bar fd) {
    return close(fd);
}

/**
 * Write until all bytes written or fatal error
 *
 * Parameters
 *     fd   - file descriptor to write to
 *     data - pointer to data buffer to be written out
 *     size - bytes to write out
 * Returns
 *     Number of bytes written or -1 if an error occurred
 */
static ssize_t persistent_write(int fd, const void *data, size_t size) {
    ssize_t res, written;
    written = 0;
    while (written < size) {
        res = write(fd, ((char*)data) + written, size - written);
        if (res == -1) {
            if (errno == EAGAIN || errno == EINTR) continue;
            else return -1;
        }
        written += res;
    }
    return written;
}

/**
 * Writes data out to the light bar
 *
 * Parameters
 *     fd   - light_bar descriptor
 *     data - pointer to data buffer to be written out
 *     size - bytes to write out
 * Returns
 *     Number of bytes written
 */
static ssize_t lb_write(light_bar fd, const void *data, size_t size) {
    int ret = persistent_write(fd, data, size);
    assert(ret == size);
    return ret;
}

light_bar lb_reset(light_bar fd) {
    lb_write(fd, &RESET, 4);
    return fd;
}

light_bar set_progress(light_bar fd, uint32_t numerator, uint32_t denominator) {
    lb_write(fd, &PROGRESS, 4);
    lb_write(fd, &numerator, 4);
    lb_write(fd, &denominator, 4);
    return fd;
}

light_bar set_random(light_bar fd) {
    lb_write(fd, &RANDOM, 4);
    return fd;
}

light_bar set_solid(light_bar fd, uint32_t color) {
    lb_write(fd, &SOLID, 4);
    lb_write(fd, &color, 4);
    return fd;
}

light_bar set_custom(light_bar fd, uint32_t *colors, size_t size) {
    lb_write(fd, &CUSTOM, 4);
    lb_write(fd, colors, 4 * size);
    return fd;
}
