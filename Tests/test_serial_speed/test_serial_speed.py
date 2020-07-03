'''
Works with test_serial_speed.ino on teensy.
'''

import serial
import time

ser = serial.Serial(port='/dev/ttyACM0', baudrate=9600)

#Time the process
start_time = time.time()
# Send a request
print('Sending request for speed')
ser.write(b'speed\n')
# Wait for return value
speed = ser.read_until()
#Stop the timer
duration = time.time() - start_time
print('Duration is:', duration, ', speed:', speed)

start_time = time.time()
ser.write(b'throttle\n')
print('\nSending throttle request')
throttle = ser.readline()
duration = time.time() - start_time
print('Duration is:', duration, ', throttle:', throttle)
