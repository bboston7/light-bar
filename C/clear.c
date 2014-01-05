#include "light_bar.h"

int main(void) {
    light_bar lb = lb_connect();
    lb_reset(lb);
    lb_close(lb);
    return 0;
}
