#!/usr/bin/python3

""" A controller for an arduino controlled light bar """

import serial

PATH = '/dev/ttyACM0'
BAUD = 115200

class LightBarController(object):
    """ Represents an arduino controlled light bar over serial """

    def __init__(self):
        ''' Initialize the light bar '''
        object.__init__(self)
        self.bar = serial.Serial(PATH, BAUD)
        ready = None
        while not ready:
            ready = self.bar.read(1)

    def __del__(self):
        ''' Clean up '''
        self.reset()
        self.bar.close()

    def _write(self, data):
        ''' Write data to the light bar '''
        self.bar.write(bytes(str(data) + '\n', 'UTF-8'))

    def reset(self):
        ''' Reset the light bar'''
        self._write(Operations.RESET)

    def set_progress(self, numerator, denominator):
        ''' Make light bar act like progress bar '''
        self._write(Operations.PROGRESS)
        self._write(numerator)
        self._write(denominator)

    def set_random(self):
        ''' Set the light bar to random colors '''
        self._write(Operations.RANDOM)

    def set_solid(self, color):
        ''' Set the light bar to a solid color '''
        self._write(Operations.SOLID)
        self._write(color)

    def set_custom(self, colors):
        """ Set the light bar to match a list of colors

        Precondition:
        len(colors) == len(light bar)

        Keyword arguments:
        colors -- A list of colors mapping each element to an LED in the light
                  bar
        """
        self._write(Operations.CUSTOM)
        for color in colors:
            self._write(color)

class Operations(object):
    """ Enum mapping light bar operations to ints """
    RESET    = 0
    SOLID    = 1
    PROGRESS = 2
    RANDOM   = 3
    CUSTOM   = 4
