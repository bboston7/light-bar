#!/usr/bin/python3

import audioop
import logging
import math
import numpy as np
import sys

from light_bar_controller import LightBarController

LEVEL_FILTER = 5

SAMPLE_FREQUENCY = 44100

# Frequency range based off of piano range
MAX_FREQ = 4186.01
MIN_FREQ = 27.5
MAX_LIN_FREQ = math.log(MAX_FREQ, 2)
MIN_LIN_FREQ = math.log(MIN_FREQ, 2)

class FreqVU(object):

    def __init__(self):
        self.bar = LightBarController()
        self.max_vol = 1
        logging.basicConfig(format='%(levelname)s:%(message)s',
                            level=logging.DEBUG)

    def __del__(self):
        self.bar.__del__()

    def update(self):
        sys.stdout.flush()
        # pylint: disable=E1101
        audio = sys.stdin.buffer.read(4096)
        audio_16 = []
        # Join bytes
        for i in range(int(len(audio)/2)):
            index = i*2
            audio_16.append(audioop.getsample(audio, 2, i) / 32768)

        # Fourier transformation
        ftt_data = np.fft.fft(audio_16)
        freq_dist = list(map(lambda x: np.sqrt(np.square(x.real) + np.square(x.imag)), ftt_data))
        freqs = freq_dist[1:int(len(freq_dist) / 2) + 1]

        for level in freqs:
            self.max_vol = max(self.max_vol, level)

        # TODO: Think about setting step and max_index in the init function
        #       This may require using constants for buffer size and bits per
        #       sample
        step = SAMPLE_FREQUENCY / len(freq_dist)
        max_index = int(math.ceil(MAX_FREQ / step))
        freqs = freqs[:max_index]
        avg_num = int(len(freqs) / 64)
        log_step = (MAX_LIN_FREQ - MIN_LIN_FREQ) / 64

        bar_data = []
        for i in range(64):
            # Linearize frequency data and populate bar_data
            min_freq = 2 ** (i * log_step + MIN_LIN_FREQ)
            max_freq = 2 ** ((i + 1) * log_step + MIN_LIN_FREQ)
            min_index = int(min_freq / step)
            max_index = int(max_freq / step)
            start_index = i * avg_num
            level = int(sum(freqs[min_index:max_index])/(max_index - min_index + 1))
            # TODO: Make this filter level configurable (removes harmonics!)
            bar_data.append(level if level > LEVEL_FILTER else 0)

        logging.debug(bar_data)

        colors = map(lambda x: int(x * (0xFFFFFF / self.max_vol)), bar_data)
        self.bar.set_custom(colors)

def main():
    freq_vu = FreqVU()
    try:
        while True:
            freq_vu.update()
    except (KeyboardInterrupt, SystemExit):
        freq_vu.__del__()
        raise

if __name__ == "__main__":
    main()
