"""
Haoru Xue | hxue@ucsd.edu | Triton-AI | http://triton-ai.eng.ucsd.edu/ | 2020

Works with a Teensy microcontroller (MCU) and RC transmitter.

In manual mode, RC controller signal (speed, throttle, steering) is sent to the Teensy before polled and stored by
the on-board computer.

In autonomous mode, speed and steering instruction is sent by the on-board computer to the Teensy, who will then
apply the appropriate throttle and steering.

Communication is done on serial.

Watchdog is in place to shutdown the system after losing connection with the Teensy for too long (100ms)

APIs:

Transmission from the on-board computer starts with either "command" or "poll", and are always terminated by '\n'
The on-board computer can poll the speed, throttle, and steering, or command the car with speed, steering or shutdown.
e.g. "command speed 314\n" "command shutdown\n" "poll steering\n" "poll\n" (which means poll everything)

Transmission from the teensy is only needed after a polling request, containing the polled attribute and value,
terminated by '\n'. If everything is polled, send separate messages for each attribute.
e.g. "steering -0.45\n" "throttle 0.758\n"
"""

import serial
from enum import Enum
import time
import sched
import re


class OperationMode(Enum):
    manual = 'M'
    auto = 'A'


class TeensyMC:
    def __init__(self, mode=OperationMode.manual):
        # Params for tweaking
        port = '/dev/ttyACM0'
        baudrate = 9600
        self.timeout = 100  # ms before watchdog kicks in
        self.pollFreq = 5  # ms between each polling

        self.running = True;
        self.ser = serial.Serial(port=port, baudrate=baudrate)
        self.mode = mode
        self.watchdog = Watchdog(threshold=self.timeout, callback=self.watchdog_alert)

        self.speed = 0.0
        self.throttle = 0.0
        self.steering = 0.0

    def update(self):
        self.__poll()
        time.sleep(self.pollFreq / 1000.0)

    def __poll(self):
        """Get input values from Teensy in manual mode"""
        while not self.ser.in_waiting:
            pass

        mcu_message = self.ser.readline().decode().lower()  # The message coming in
        sbc_message = 'poll'  # The message to be sent back. a single 'poll' means polling every information
        number_in_message = re.findall(r'\d+\.*\d*', mcu_message)  # Find number in message

        if number_in_message is not None:
            self.watchdog.reset_countdown()  # Reset watchdog as soon as data is received

            if 'speed' in mcu_message:
                self.speed = number_in_message[0]
                sbc_message += ' speed'
            elif 'throttle' in mcu_message:
                self.throttle = number_in_message[0]
                sbc_message += ' throttle'
            elif 'steering' in mcu_message:
                self.steering = number_in_message[0]
                sbc_message += ' steering'

        self.ser.write(bytes(sbc_message + '\n'))

    def __command(self, speed=0, steering=0, shutdown=False):
        """Send Instructions to Teensy in auto mode"""
        if not shutdown:
            self.ser.write(b'command speed ' + bytes(speed) + b'\n')
            self.ser.write(b'command steering ' + bytes(steering) + b'\n')
        else:
            self.ser.write(b'command shutdown\n')

    def run_threaded(self, *throttle_and_steering):
        if self.mode == OperationMode.auto and len(throttle_and_steering) == 2:
            self.speed = throttle_and_steering[0]
            self.steering = throttle_and_steering[1]
            self.__command(self.speed, self.steering)

        return self.speed, self.throttle, self.steering  # Be very careful with the order

    def shutdown(self):
        if self.running:
            self.running = False
            self.__command(shutdown=True)
            self.ser.close()
            self.watchdog.shutdown()

    def watchdog_alert(self):
        """Callback function when watchdog is alarmed"""
        self.shutdown()


class Watchdog:
    """Trigger the callback function if timer reaches threshold (ms), unless reset"""

    def __init__(self, threshold=100, callback=None):
        self.threshold = threshold / 1000.0
        self.timeElapsed = None
        self.callback = callback
        self.sche = sched.scheduler(time.time, time.sleep)

    def start_watchdog(self, delay=500):
        """Start the watchdog countdown after the delay (ms)"""
        delay_second = delay / 1000.0
        self.sche.enter(delay_second, 1, self.__watching)

    def __watching(self):
        self.sche.enter(self.threshold, 1, self.callback)

    def reset_countdown(self):
        """Reset the watchdog countdown"""
        list(map(self.sche.cancel, self.sche.queue))
        self.__watching()

    def shutdown(self):
        """End the watchdog"""
        list(map(self.sche.cancel, self.sche.queue))
