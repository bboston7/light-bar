#!/usr/bin/python3

import audioop
import logging
import sys

from light_bar_controller import LightBarController
from collections import deque

bar = LightBarController()

total_peak = 0

last = deque()

logging.basicConfig(format='%(levelname)s:%(message)s', level=logging.DEBUG)

while True:

    var = sys.stdin.buffer.read(1024)

    peak = audioop.avgpp(var, 2)
    last.append(peak)

    if len(last) == 6:
        last.popleft()
        peak = int(sum(last)/len(last))

    # In my testing, it's extreamly rare for an actual audio source to output
    # nothing.  Thus, this is safe and we will only re-calibrate when switching
    # songs/audio sources
    if peak == 0 and total_peak != 0:
        total_peak = 0
        logging.debug('0 / 0')
        bar.reset
    elif peak != 0:
        total_peak = max(peak, total_peak)
        bar.set_progress(peak, total_peak)
        logging.debug(str(peak) + ' / ' + str(total_peak))
