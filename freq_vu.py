#!/usr/bin/python3

import audioop
import math
import numpy as np
import struct
import sys

from light_bar_controller import LightBarController
from time import sleep

SAMPLE_FREQUENCY = 44100

# Frequency range based off of piano range
MAX_FREQ = 4186.01
MIN_FREQ = 27.5
MAX_LIN_FREQ = math.log(MAX_FREQ, 2)
MIN_LIN_FREQ = math.log(MIN_FREQ, 2)

bar = LightBarController()

max_peak = 0

'''
byte = b'\x00'
while byte == b'\x00':
    byte = sys.stdin.buffer.read(1)
    #print(byte)

sys.stdin.buffer.read()
'''

max_vol = 1
min_start = None
max_end = None

while True:

#for i in range(20):
    sys.stdout.flush()
    audio = sys.stdin.buffer.read(4096)
    audio_16 = []
    # Join bytes
    for i in range(int(len(audio)/2)):
        index = i*2
        '''
        #unit = (audio[index] << 8) + audio[index+1]
        unit = struct.unpack('<h', audio[index])
        audio_16.append(unit)
        '''
        audio_16.append(audioop.getsample(audio, 2, i) / 32768)
    #print(audio_16)

    #print(audio)
    ftt_data = np.fft.fft(audio_16)
    #print(ftt_data)
    freq_dist = list(map(lambda x: np.sqrt(np.square(x.real) + np.square(x.imag)), ftt_data))
    #print(freq_dist)
    freqs = freq_dist[1:int(len(freq_dist) / 2) + 1]
    #print(freqs)

    for level in freqs:
        max_vol = max(max_vol, level)
    step = SAMPLE_FREQUENCY / len(freq_dist)
    max_index = int(math.ceil(MAX_FREQ / step))
    freqs = freqs[:max_index]
    bar_data = []
    avg_num = int(len(freqs) / 64)
    log_step = (MAX_LIN_FREQ - MIN_LIN_FREQ) / 64
    for i in range(64):
        min_freq = 2 ** (i * log_step + MIN_LIN_FREQ)
        max_freq = 2 ** ((i + 1) * log_step + MIN_LIN_FREQ)
        min_index = int(min_freq / step)
        max_index = int(max_freq / step)
        start_index = i * avg_num
        level = int(sum(freqs[min_index:max_index])/(max_index - min_index + 1))
        # TODO: Make this filter level configurable (removes harmonics!)
        bar_data.append(level if level > 5 else 0)



    print(bar_data)

    colors = map(lambda x: int(x * (0xFFFFFF / max_vol)), bar_data)
    #print(list(colors))
    bar.set_custom(colors)
