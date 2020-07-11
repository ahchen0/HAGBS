#!/usr/bin/env python3

import smbus
import time

bus = smbus.SMBus(1)
address = 0x0A

channels = [0x88, 0xC8, 0x98, 0xD8, 0xA8, 0xE8, 0xB8, 0xF8]
f = open("/home/pi/lite/data/adc.csv", "a+")
f.write("(Board Labelled) LM1, LM2, LM3, LM4, LM5, LM6\n")
while True:
    dataArr = []
    for i in range(len(channels)):
        try:
            data = bus.read_word_data(address, channels[i])
            top = (data & 0xFF) << 4
            bot = (data & 0xF000) >> 12
            dataArr.append(top+bot)
        except:
            break
    for i in range(len(dataArr)):
        dataArr[i] = dataArr[i]*(-85.54319)/1000+159.6493
    f.write(str(time.time()))
    f.write(", ")
    f.write(str(dataArr[2]))
    f.write(", ")
    f.write(str(dataArr[1]))
    f.write(", ")
    f.write(str(dataArr[3]))
    f.write(", ")
    f.write(str(dataArr[4]))
    f.write(", ")
    f.write(str(dataArr[6]))
    f.write(", ")
    f.write(str(dataArr[5]))
    f.write("\n")
    time.sleep(10)
