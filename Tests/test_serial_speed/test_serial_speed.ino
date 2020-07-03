//************************
//Run this code in teensy. Works with test_serial_speed.py on Jetson Nano
//************************

float speed_of_car = 3.14;
float throttle = 123.4;
String input;

void setup() {
  Serial.begin(9600);
}

void loop() {
  //Awaiting request
  while (Serial.available() == 0);

  //Read request
  input = Serial.readStringUntil('\n');

  //Send back data
  if (input == String("speed")){
    Serial.print(speed_of_car);
  }
  else if (input == String("throttle")){
    Serial.print(throttle);
  }
  
  Serial.print('\n');
}
