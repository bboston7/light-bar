#!/usr/bin/python3

from time import sleep

from light_bar_controller import LightBarController

bar = LightBarController()

green = list(map(lambda x: 0xFF0000, range(64)))
red = list(map(lambda x: 0x00FF00, range(64)))
blue = list(map(lambda x: 0x0000FF, range(64)))

sdur = 1/15

while True:
    # TODO: To make more efficient, use set solid, rather than set_custom
    bar.set_custom(red)
    print("red")
    sleep(sdur)
    bar.set_custom(green)
    print("green")
    sleep(sdur)
    bar.set_custom(blue)
    print("blue")
    sleep(sdur)
