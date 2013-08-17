#!/usr/bin/python3

from light_bar_controller import LightBarController
from time import sleep

bar = LightBarController()

for i in range(0, 5000):
    bar.set_progress(i, 5000)
    print(i)
    sleep(.01)
