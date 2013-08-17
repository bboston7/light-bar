#!/usr/bin/python3.3

import audioop
import subprocess

from light_bar_controller import LightBarController
from collections import deque

bar = LightBarController()

total_peak = 0

last = deque()

while True:
    try:
        subprocess.check_output(['pacat', '-rd', 'alsa_output.pci-0000_00_1b.0.analog-stereo.monitor', '--channels=1'], timeout=.04)
    except subprocess.TimeoutExpired as e:
        var = e.output

    peak = audioop.avgpp(var, 2)
    last.append(peak)

    if len(last) == 6:
        last.popleft()
        peak = int(sum(last)/len(last))

    total_peak = max(peak, total_peak)
    bar.set_progress(peak, total_peak)
    print(peak, '/', total_peak)
