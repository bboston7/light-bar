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
        self.pallett = Pastel.REVERSE_RAINBOW

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

            for level in freqs:
                self.max_vol = max(self.max_vol, level)

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
                # TODO: Make this filter level configurable (removes harmonics!)
                bar_data.append(level if level > LEVEL_FILTER else 0)

            logging.debug(bar_data)

            colors = map(lambda x: int(x * (0xFFFFFF / self.max_vol)), bar_data)
            self.bar.set_custom(colors)

class Pastel(object):
    """ Pastel color constants """
    RED             = 0xF7977A
    RED_ORANGE      = 0xF9AD81
    YELLOW_ORANGE   = 0xFDC68A
    YELLOW          = 0xFFF79A
    PEA_GREEN       = 0xC4DF9B
    YELLOW_GREEN    = 0xA2D39C
    GREEN           = 0x82CA9D
    GREEN_CYAN      = 0x7BCDC8
    CYAN            = 0x6ECFF6
    CYAN_BLUE       = 0x7EA7D8
    BLUE            = 0x8493CA
    BLUE_VIOLET     = 0x8882BE
    VIOLET          = 0xA187BE
    VIOLET_MAGENTA  = 0xBC8DBF
    MAGENTA         = 0xF49AC2
    MAGENTA_RED     = 0xF6989D
    REVERSE_RAINBOW = [VIOLET, BLUE_VIOLET, BLUE, CYAN_BLUE, CYAN, GREEN_CYAN,
                       GREEN, YELLOW_GREEN, PEA_GREEN, YELLOW, YELLOW_ORANGE,
                       RED_ORANGE, RED, 0xFFFFFF]
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
