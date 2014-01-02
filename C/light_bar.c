#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
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
    // TODO: Do I need O_NOCITY or O_NONBLOCK?
    light_bar ret = open(PATH, O_RDWR);
    assert(ret != -1);

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
static int lb_write(light_bar fd, const void *data, size_t size) {
    int res, written;
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

light_bar set_progress(light_bar fd, int numerator, int denominator) {
    int res;
    res = lb_write(fd, &PROGRESS, 4);
    assert(res == 4);
    lb_write(fd, &numerator, 4);
    assert(res == 4);
    lb_write(fd, &denominator, 4);
    assert(res == 4);
    return fd;
}
