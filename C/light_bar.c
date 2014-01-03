#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include "light_bar.h"

#define BAUD 115200
#define PATH "/dev/ttyACM0"

static const int RESET    = 0;
static const int SOLID    = 1;
static const int PROGRESS = 2;
static const int RANDOM   = 3;
static const int CUSTOM   = 4;


light_bar lb_connect() {
    int res;
    // TODO: Do I need O_NOCTTY?
    light_bar ret = open(PATH, O_RDWR | O_NOCTTY);
    assert(ret != -1);

    // http://chrisheydrick.com/2012/06/24/how-to-read-serial-data-from-an-arduino-in-linux-with-c-part-4/
    // TODO: Set baud and c_cflags
    struct termios opts;
    tcgetattr(ret, &opts);
    // Set baud
    res = cfsetspeed(&opts, BAUD);
    assert(res == 0);
    // 8 bits, no parity, no duplicate stop bits
    opts.c_cflag &= ~(PARENB | CSTOPB | CSIZE);
    opts.c_cflag |= CS8;

    // TODO: Canonical or noncanonical?  I think noncanonical.  I only ever do
    // one read, maybe it doesn't matter?

    // TODO: Does the read check still need to be in a while loop if O_NONBLOCK
    // isn't set?
    char ready = 0;
    while (ready != 1) {
        read(ret, &ready, 1);
    }

    return ret;
}

int lb_close(light_bar fd) {
    return close(fd);
}

/**
 * Writes data out to the light bar
 *
 * Parameters
 *     fd   - light_bar descriptor
 *     data - pointer to data buffer to be written out
 *     size - bytes to write out
 * Returns
 *     Number of bytes written or -1 if an error occurred
 */
static ssize_t lb_write(light_bar fd, const void *data, size_t size) {
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

light_bar lb_reset(light_bar fd) {
    int res = lb_write(fd, &RESET, 4);
    assert(res == 4);
    return fd;
}

light_bar set_progress(light_bar fd, uint32_t numerator, uint32_t denominator) {
    ssize_t res;
    res = lb_write(fd, &PROGRESS, 4);
    assert(res == 4);
    lb_write(fd, &numerator, 4);
    assert(res == 4);
    lb_write(fd, &denominator, 4);
    assert(res == 4);
    return fd;
}

light_bar set_random(light_bar fd) {
    int res = lb_write(fd, &RANDOM, 4);
    assert(res == 4);
    return fd;
}

light_bar set_solid(light_bar fd, uint32_t color) {
    int res = lb_write(fd, &SOLID, 4);
    assert(res == 4);
    res = lb_write(fd, &color, 4);
    assert(res == 4);
    return fd;
}

light_bar set_custom(light_bar fd, uint32_t *colors, size_t size) {
    int res = lb_write(fd, &CUSTOM, 4);
    assert(res == 4);
    for (size_t i = 0; i < size; ++i) {
        res = lb_write(fd, colors+i, 4);
        assert(res == 4);
    }
    return fd;
}
