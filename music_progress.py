#!/usr/bin/python3

import datetime
import subprocess
import time

from light_bar_controller import LightBarController
from time import sleep

bar = LightBarController()

while True:
    # TODO: Take data from time, rather than percentage
    # Also, cache results
    percentage = str(subprocess.check_output(['mpc']), encoding='utf-8')
    percentage = percentage.split('\n')[1]
    percentage = percentage.split(' ')[-2]
    times = percentage.split('/')
    current = time.strptime(times[0], '%M:%S')
    length = time.strptime(times[1], '%M:%S')
    current = datetime.timedelta(minutes=current.tm_min, seconds=current.tm_sec).total_seconds()
    length = datetime.timedelta(minutes=length.tm_min, seconds=length.tm_sec).total_seconds()
    bar.set_progress(int(current), int(length))
    print(current, '/', length)
    sleep(1)
