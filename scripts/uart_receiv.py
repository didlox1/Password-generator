import serial
import numpy
import matplotlib.pyplot as plt

mcu = serial.Serial('COM12', 115200, timeout=0.1)
rawPasswords = []
while True:
    data = mcu.readline()
    if data:
        rawPass = data.decode()
        print(rawPass)
        rawPasswords.append(rawPass)