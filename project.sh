#!/bin/bash
make
insmod project5.ko
dmesg -c > /dev/null
cyclictest -m -p90  -l100000 -q
rmmod project5.ko
dmesg > dmesg.txt
python user_space.py
