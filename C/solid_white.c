#include <unistd.h>
#include "light_bar.h"

int main() {
    light_bar fd = lb_connect();
    set_solid(fd, 0xFFFFFF);
    lb_close(fd);
}
