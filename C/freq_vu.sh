#!/bin/sh

pacat -rd alsa_output.pci-0000_00_1b.0.analog-stereo.monitor --channels=1 --raw --latency=1024 | ./freq_vu
