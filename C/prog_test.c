#include <stdio.h>
#include <unistd.h>
#include "light_bar.h"

#define RUNS 100

int main(void) {
    light_bar lb = lb_connect();

    for (int i = 0; i <= RUNS; ++i) {
        set_progress(lb, i, RUNS);
        printf("%d/%d\n", i, RUNS);
        usleep(100000);
    }
    lb_close(lb);

    return 0;
}
