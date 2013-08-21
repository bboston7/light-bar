#!/usr/bin/python3

import audioop
import logging
import math
import numpy as np
import pyfftw
import sys

from light_bar_controller import LightBarController
from time import sleep

LEVEL_FILTER = 0

SAMPLE_FREQUENCY = 44100

# Frequency range based off of piano range
MAX_FREQ = 4186.01
MIN_FREQ = 27.5 # TODO: This seems to be placed too high on the bar.  Investigate
MAX_LIN_FREQ = math.log(MAX_FREQ, 2)
MIN_LIN_FREQ = math.log(MIN_FREQ, 2)

class FreqVU(object):
    """
    FreqVU object causes the light bar to act such that each LED represents one
    fraction of the frequency range between MIN_FREQ and MAX_FREQ.  The color
    of each LED is determined by the level of that particular frequency range
    """

    def __init__(self):
        """ Setup the frequency vu """
        self.bar = LightBarController()
        self.max_vol = 1
        logging.basicConfig(format='%(levelname)s:%(message)s',
                            level=logging.DEBUG)
        self.pallett = RGB.REVERSE_RAINBOW

    def __del__(self):
        """ Clean up """
        self.bar.__del__()

    def update(self):
        """ Update the LEDS of the bar to match the PCM input """
        sys.stdout.flush()
        # pylint: disable=E1101
        audio = sys.stdin.buffer.read(4096)
        audio_16 = []
        # Join bytes
        for i in range(int(len(audio)/2)):
            audio_16.append(audioop.getsample(audio, 2, i) / 32768)

        # Fourier transformation
        # TODO: Better variable names?
        ftt_data = np.fft.fft(audio_16)
        if ftt_data[0]:
            freq_dist = list(map(
                lambda x: np.sqrt(np.square(x.real) + np.square(x.imag)), ftt_data))
            freqs = freq_dist[1:int(len(freq_dist) / 2) + 1]

            '''
            for level in freqs:
                self.max_vol = max(self.max_vol, level)
            print(self.max_vol)
            '''
            # TODO: Think about setting step and max_index in the init function
            #       This may require using constants for buffer size and bits per
            #       sample
            step = SAMPLE_FREQUENCY / len(freq_dist)
            max_index = int(math.ceil(MAX_FREQ / step))
            freqs = freqs[:max_index]
            log_step = (MAX_LIN_FREQ - MIN_LIN_FREQ) / 64

            bar_data = []
            for i in range(64):
                # Linearize frequency data and populate bar_data
                min_freq = 2 ** (i * log_step + MIN_LIN_FREQ)
                max_freq = 2 ** ((i + 1) * log_step + MIN_LIN_FREQ)
                min_index = int(min_freq / step)
                max_index = int(max_freq / step)
                level = int(sum(freqs[min_index:max_index])/(max_index - min_index + 1))
                self.max_vol = max(self.max_vol, level)
                # TODO: Make this filter level configurable (removes harmonics!)
                bar_data.append(level if level > LEVEL_FILTER else 0)

            logging.debug(bar_data)
            print(self.max_vol)

            # TODO: Refine color selection
            colors = map(lambda x: self._get_color(x), bar_data)
            self.bar.set_custom(colors)

        elif self.max_vol != 1:
            self.max_vol = 1
            self.bar.reset()

    def _get_color(self, level):
        # TODO: Play around with disabling this on an expanded pallet to act
        #       as a harmonic filter
        if level == 0:
            return 0x000000
        index = int((level * 127) / self.max_vol)
        '''
        index = int((level * len(self.pallett)) / self.max_vol)
        if index == len(self.pallett):
            return 0xFFFFFF
        '''
        return index * 0x010101

class Pastel(object):
    """ Pastel color constants """
    RED             = 0x97F77A
    RED_ORANGE      = 0xADF981
    YELLOW_ORANGE   = 0xC6FD8A
    YELLOW          = 0xF7FF9A
    PEA_GREEN       = 0xDFC49B
    YELLOW_GREEN    = 0xD3A29C
    GREEN           = 0xCA829D
    GREEN_CYAN      = 0xCD7BC8
    CYAN            = 0xCF6EF6
    CYAN_BLUE       = 0xA77ED8
    BLUE            = 0x9384CA
    BLUE_VIOLET     = 0x8288BE
    VIOLET          = 0x87A1BE
    VIOLET_MAGENTA  = 0x8DBCBF
    MAGENTA         = 0x9AF4C2
    MAGENTA_RED     = 0x98F69D
    REVERSE_RAINBOW = [0x000000, VIOLET, BLUE_VIOLET, BLUE, CYAN_BLUE, CYAN, GREEN_CYAN,
                       GREEN, YELLOW_GREEN, PEA_GREEN, YELLOW, YELLOW_ORANGE,
                       RED_ORANGE, RED]
    RAINBOW         = REVERSE_RAINBOW[::-1]
    RAINBOW.remove(0)
    RAINBOW.insert(0, 0)

class RGB(object):
    """ RGB Color constants """
    RED     = 0x00FF00
    YELLOW  = 0xFFFF00
    GREEN   = 0xFF0000
    CYAN    = 0xFF00FF
    BLUE    = 0x0000FF
    MAGENTA = 0x00FFFF
    REVERSE_RAINBOW = [0x000000, MAGENTA, BLUE, CYAN, GREEN, YELLOW, RED]


def main():
    """ main function runs the program """
    freq_vu = FreqVU()
    try:
        while True:
            freq_vu.update()
    except (KeyboardInterrupt, SystemExit):
        freq_vu.__del__()
        raise

if __name__ == "__main__":
    main()
