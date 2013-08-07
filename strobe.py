#!/usr/bin/python3


from time import sleep
from light_bar_controller import LightBarController

bar = LightBarController()

while(True):
    bar.set_solid(0xFFFFFF)
    sleep(0.1)
    bar.reset()
    sleep(0.1)
