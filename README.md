# teensy-JTN-API
## Purpose

Communicate between the single-board computer (Jetson Nano) and a teensy microcontroller to control the peripherials such as electronic speed controller (ESC) and servo. Retrieve speedometer readings from the brushless sensored motor.

### Serial
0.5~1 ms transmission of small literal message

All communications are initiated by the Jetson Nano (JTN). It will send instructions to the Teensy (starting with “command”), or poll data from the Teensy (starting with “poll”).

## Commanding

### Speed and steering control 

JTN: “command speed 32324\n”
JTN: “command steering 43544\n”

### Shutdown

JTN: “command shutdown\n”
  
## Polling

### Speed

JTN: “poll speed\n”
Teensy: “speed 12345\n”
  
### Throttle

JTN: “poll throttle\n”
Teensy: “throttle 256\n” (PWM value)
  
### Steering

JTN: “poll steering\n”
Teensy: “steering 563\n” (PWM value)
  
### All
JTN: “poll\n”
Teensy: [send all data in three separate messages]
  
### Watchdog
Wait for up to [TO BE DETERMINED] between each serial transmission. Otherwise, shutdown.

