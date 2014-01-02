typedef int light_bar;

/**
 * Connects to the light_bar
 *
 * Returns
 *     light_bar signifying the connection to the light bar
 */
light_bar lb_connect();

/**
 * Closes the light_bar
 *
 * Parameters
 *     fd - Light bar connection to close
 * Returns
 *     Zero on success, -1 on error.  errno set by close in unistd.h
 */
int lb_close(light_bar fd);

/**
 * Clears the light bar
 *
 * Parameters
 *     fd - Light bar to clear
 * Returns
 *     fd
 */
int lb_reset(light_bar fd);

/**
 * Make light bar act like a progress bar
 *
 * Parameters
 *     fd          - light bar to alter
 *     numerator   - numerator of progress fraction
 *     denominator - denominator of progress fraction
 * Returns
 *     fd
 */
int set_progress(light_bar fd, int numerator, int denominator);
