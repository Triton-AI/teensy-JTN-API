//************************
//Run this code in teensy. Works with Microcontroller.py on Jetson Nano
//Connect USB.
//************************

float speed_of_car = 3.14;//dummy, in development
float throttle = 0.0;
float steering = 0.0;
int throttle_pwm = 0;
int steering_pwm = 0;
String input; //input from serial

//pins
int servoPin = 5;
int escPin = 6;

//boundary values
int zero_throttle_pwm = 370;
int center_steering = 450;
int max_throttle_pwm = 405;
int min_throttle_pwm = 330;
int min_steering_pwm = 335;
int max_steering_pwm = 545;

bool isSafeThrottle(int throttle_pwm){
  return throttle_pwm >= min_throttle_pwm && throttle_pwm <= max_throttle_pwm;
}

bool isSafeSteering(int steering_pwm){
  return steering_pwm >= min_steering_pwm && steering_pwm <= max_steering_pwm;
}

int mapPWM(float input, int max_pwm, int min_pwm){
  //Take input [-1,1] and map it to pwm
  int center_pwm = 0;
  center_pwm = (max_pwm + min_pwm) / 2;
  return center_pwm + (int)((center_pwm - min_pwm) * input);
}

void setup() {
  Serial.begin(115200);
  analogWriteResolution(12);//4096 resolution
  analogWriteFrequency(servoPin, 60); //60Hz
  analogWriteFrequency(escPin, 60);
  pinMode(servoPin, OUTPUT);
  pinMode(escPin, OUTPUT);

  //initialize pwm to stable position
  analogWrite(servoPin, center_steering);
  analogWrite(escPin, zero_throttle_pwm);
}

void loop() {
  //Awaiting request
  while (Serial.available() == 0);

  //Read request
  input = Serial.readStringUntil('\n');

  //Process input
  if (input.indexOf("poll") >= 0){

    if (input.indexOf("speed") >= 0){
      Serial.print("speed ");
      Serial.print(speed_of_car);
    }
    else{ //poll all
      Serial.print("speed ");
      Serial.print(speed_of_car);
    }
      Serial.print('\n');
  }
  else if (input.indexOf("command") >= 0){
    int thr_pos = input.indexOf("throttle");
    int ste_pos = input.indexOf("steering");
    if (thr_pos >= 0){
      throttle = input.substring(thr_pos + 9).toFloat();
      throttle_pwm = mapPWM(throttle, max_throttle_pwm, min_throttle_pwm);
      if (isSafeThrottle(throttle_pwm))
        analogWrite(escPin, throttle_pwm);
       else
        analogWrite(escPin, zero_throttle_pwm);
        
      //Serial.print("throttle set to ");
      //Serial.print(throttle);
    }
    else if (ste_pos >= 0){
      steering = input.substring(ste_pos + 9).toFloat();
      steering_pwm = mapPWM(throttle, max_steering_pwm, min_steering_pwm);
      if (isSafeSteering(steering_pwm))
        analogWrite(servoPin, steering_pwm);
       else
        analogWrite(servoPin, center_steering);
        
      //Serial.print("steering set to ");
      //Serial.print(steering);
    }
  }
}
