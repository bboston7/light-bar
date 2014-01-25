#include <assert.h>
#include <complex.h>
#include <fftw3.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tgmath.h>
#include "light_bar.h"

#define BAR_SIZE 64
#define LEVEL_FILTER 1
#define SAMPLE_FREQUENCY 44100
#define FFT_LEN 8192
#define SAMPLE_SIZE 2
static const unsigned int SAMPLES = 2048;

// Frequency range based off of piano range
#define MAX_FREQ 4186.01
#define MIN_FREQ 27.5
static const double MAX_LIN_FREQ = log2(MAX_FREQ);
static const double MIN_LIN_FREQ = log2(MIN_FREQ);

#define FUDGE_RANGE true

typedef struct {
    light_bar bar;
    double max_vol;
    fftw_complex *data;
    fftw_plan plan;
    double step;
    unsigned int freq_size;
    double log_step;
} state;

/**
 * Initialize the state of the computation
 *
 * Returns
 *     State struct to send to update
 */
state init() {
    state this;
    this.bar = lb_connect();
    this.max_vol = 1;

    // Prepare for FFT
    this.data = fftw_malloc(sizeof(fftw_complex) * FFT_LEN);
    assert(this.data != NULL);
    memset(this.data, 0, sizeof(fftw_complex) * FFT_LEN);
    this.plan = fftw_plan_dft_1d(FFT_LEN, this.data, this.data, FFTW_FORWARD,
                                 FFTW_PATIENT);

    // Pre-calculate size of frequency array
    this.step = SAMPLE_FREQUENCY / (FFT_LEN / 2);
    int max_index = ceil(MAX_FREQ / this.step);
    this.freq_size = max_index < FFT_LEN / 2 ? max_index : FFT_LEN / 2;

    // Calculate log step
    this.log_step = (MAX_LIN_FREQ - MIN_LIN_FREQ) / BAR_SIZE;

    return this;
}

/**
 * Sum arr from mindex (inclusive) to maxdex (exclusive)
 */
double sum(double *arr, int mindex, int maxdex) {
    double ret = 0;
    for (int i = mindex; i < maxdex; ++i) {
        ret += arr[i];
    }
    return ret;
}

/**
 * Get the appropriate color for the level
 */
uint8_t get_color(state *this, double level) {
    int index = (level * 0xFF) / this->max_vol;
    return index;
    //return index * 0x010101;
}

void print_arr(uint8_t *arr) {
    printf("[");
    for (int i = 0; i < 64; ++i) {
        printf("%X,", arr[i]);
    }
    printf("]\n");
}

void print_arrd(double *arr) {
    printf("[");
    for (int i = 0; i < 64; ++i) {
        printf("%lf,", arr[i]);
    }
    printf("]\n");
}

void print_arrc(double complex *arr) {
    printf("[");
    for (int i = 0; i < 64; ++i) {
        printf("%lf + %lfi, ", creal(arr[i]), cimag(arr[i]));
    }
    printf("]\n");
}

void update(state *this) {
    // TODO: Separate input and output arrays so there is no need to clear
    memset(this->data, 0, sizeof(fftw_complex) * FFT_LEN);
    int res;
    // Read and process sample from stdin
    for (int i = 0; i < SAMPLES; ++i) {
        int16_t sample;
        res = fread(&sample, SAMPLE_SIZE, 1, stdin);
        assert(res == 1);
        // Reduce sample to range [0,1]
        double complex csample = ((double complex) sample) / INT16_MAX;
        this->data[i] = csample;
    }

    // Perform the fast Fourier transform
    fftw_execute(this->plan);

    #ifndef NDEBUG
    //print_arrc(this->data);
    #endif

    if (this->data[0]) {
        // Get frequency data from the transformation
        double freqs[this->freq_size];
        for (unsigned int i = 1; i < this->freq_size + 1; ++i) {
            double complex freq = this->data[i];
            freqs[i - 1] = sqrt(pow(creal(freq), 2) + pow(cimag(freq), 2));
        }

        #ifndef NDEBUG
        //print_arrd(freqs);
        #endif

        double bar_data[BAR_SIZE];
        for (int i = 0; i < BAR_SIZE; ++i) {
            // Linearize frequency data and populate bar_data
            double min_freq = pow(2, i * this->log_step + MIN_LIN_FREQ);
            double max_freq = pow(2, (i + 1) * this->log_step + MIN_LIN_FREQ);
            unsigned int min_index = (unsigned int) min_freq / this->step;
            unsigned int max_index = (unsigned int) max_freq / this->step;
            if (FUDGE_RANGE && max_index == min_index)
                // Mark data point to be fudged
                bar_data[i] = -1;
            else {
                double level = sum(freqs, min_index, max_index) / (max_index - min_index + 1);
                int check_index = 1;
                while (i - check_index >= 0 && bar_data[i - check_index] < 0) {
                    // Fudge data point
                    double prev_level = level - check_index;
                    bar_data[i - check_index] = prev_level > 0 ? prev_level : 0;
                    ++check_index;
                }
                level = level - LEVEL_FILTER;
                if (level < 0) level = 0;
                if (level > this->max_vol) this->max_vol = level;
                bar_data[i] = level;
                //bar_data[i] = level >= LEVEL_FILTER ? level : 0;
            }
        }

        #ifndef NDEBUG
        printf("max_vol: %lf\n", this->max_vol);
        print_arrd(bar_data);
        #endif

        // TODO: Just do this instead of building bar_data array?  Removes a
        // pass from the data
        // Build color array
        //uint8_t colors[BAR_SIZE];
        for (int i = 0; i < BAR_SIZE; ++i) {
            //colors[i] = get_color(this, bar_data[i]);
            uint8_t color = get_color(this, bar_data[i]);
            set_pixel(this->bar, i, color);
        }

        #ifndef NDEBUG
        //print_arr(colors);
        #endif

        //set_custom(this->bar, colors, BAR_SIZE);

    } else if (this->max_vol > 1) {
        this->max_vol = 1;
        // Zero out bar
        for (int i = 0; i < BAR_SIZE; ++i) {
            set_pixel(this->bar, i, 0);
        }
    }
}

int main(void) {
    printf("Initializing state...");
    fflush(stdout);
    state this = init();
    printf("done\n");
    while (1) {
        update(&this);
    }

    return 0;
}
