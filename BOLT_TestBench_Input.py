import serial
import sys
import csv
import time
import os

dataNames = ['Front Wheel Speed', 'Rear Wheel Speed', 'Front Suspension', 'Rear Suspension',
                      'Brake Switch 1', 'Brake Switch 2', 'No TC', 'Low TC', 'High TC',
                      'Throttle Switch']
# ****************** Open Serial Port ****************** #
ser = serial.Serial()
ser.baudrate = 115200  # int(input("Provide a Baudrate (ex. 9600, 115200, etc.):\t"))
ser.port = 'COM' + (input("Provide a COM Port (ex. 6, 7, 8, etc.):\t"))
ser.xonxoff = True
# ****************** Open Serial Port ****************** #

"""
operating under the assumption that:
byte size is 8
parity is off
stop bit is 1
baud rate is 115200
"""
# ****************** CSV Opening ****************** #
while True:
    input_filename = input("Provide a .csv file (ex. input.csv):\t")
    if os.path.isfile(input_filename):
        input_file = open(input_filename, 'r')
        break
# ****************** CSV Opening ****************** #

# ****************** CSV Reading & TXT Writing ****************** #
with open("BOLT_TestBench_Output.txt", "w") as output_file:
    text = input_file.read()
    for line in text.splitlines():
        print(line)
        line_data = line.split(",")
        for i in range(0, len(line_data)):
            #input("Press Enter to send data")
            ser.open()
            data = line_data[i]
            output_file.write(dataNames[i] + ": " + data + "\n")
            for j in range(3 - len(data)):
                data += '\0'
            ser.write(data.encode())
            if (i != len(line_data) - 1):
                ser.close()
        while (ser.in_waiting == 0):
            print("Waiting for Throttle Input from C2000.........")
            time.sleep(1)
        output = int(ser.read(11).decode("utf-8"))
        print(str(output))
        output_file.write("Throttle Input: " + str(output) + "\n")
        while (ser.in_waiting == 0):
            print("Waiting for ETCM Request from C2000.........")
            time.sleep(1)
        output = int(ser.read(11).decode("utf-8"))
        print(str(output))
        output_file.write("ETCM Request: " + str(output) + "\n\n")
        ser.close()
# ****************** CSV Reading & TXT Writing ****************** #

    input_file.close()

"""
Basis for README.txt...

Shorthand:
FW Sp -> Front Wheel Speed
BW Sp -> Back Wheel Speed
FW Sus -> Front Wheel Suspension
BW Sus -> Back Wheel Suspension
BS1 -> Brake Switch 1
BS2 -> Brake Switch 2
PS1 -> Profile Switch 1
PS2 -> Profile Switch 2
PS3 -> Profile Switch 3
TS -> Throttle Switch
TR -> Throttle Request

The structure of .csv input is as follows per row:
FW Sp, BW Sp, FW Sus, BW Sus, BS1, BS2, TS

The structure of .txt output is as follows per line:
FW Sp
BW Sp
FW Sus
BW Sus
BS1
BS2
PS1
PS2
PS3
TS
TR

"""
