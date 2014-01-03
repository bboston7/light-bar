#pragma once

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
light_bar lb_reset(light_bar fd);

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
light_bar set_progress(light_bar fd, uint32_t numerator, uint32_t denominator);

/**
 * Set the light bar to a solid color
 *
 * Parameters
 *     fd    - light bar to alter
 *     color - color to set bar to
 * Returns
 *     fd
 */
light_bar set_solid(light_bar fd, uint32_t color);

/**
 * Set the light bar to match a list of colors
 *
 * Requires
 *     size == length of light bar
 * Parameters
 *     fd     - light bar to alter
 *     colors - array of colors mapping each element to an LED in the light bar
 *     size   - size of colors array
 * Returns
 *     fd
 *
 */
light_bar set_custom(light_bar fd, uint32_t *colors, size_t size);
