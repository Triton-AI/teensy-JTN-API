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
int center_steering_pwm = 450;
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

int mapPWM(float input, int max_pwm, int mid_pwm, int min_pwm){
  //Take input [-1,1] and map it to pwm

  if (input == 0)
    return mid_pwm;

  else if (input < 0)
    return (int)((mid_pwm) + (mid_pwm - min_pwm) * input);

  else if (input > 0)
    return (int)((mid_pwm) + (max_pwm - mid_pwm) * input);
}

int getIntAfter(String words, String subword){
  return words.substring(words.indexOf(subword) + subword.length()).toInt();
}

bool stringContains(String words, String toMatch){
  return words.indexOf(toMatch) >= 0;
}

void setup() {
  Serial.begin(115200);
  analogWriteResolution(12);//4096 resolution
  analogWriteFrequency(servoPin, 60); //60Hz
  analogWriteFrequency(escPin, 60);
  pinMode(servoPin, OUTPUT);
  pinMode(escPin, OUTPUT);

  //initialize pwm to stable position
  analogWrite(servoPin, center_steering_pwm);
  analogWrite(escPin, zero_throttle_pwm);
}

void loop() {
  //Awaiting request
  while (Serial.available() == 0);

  //Read request
  input = Serial.readStringUntil('\n').toLowerCase();

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
      throttle_pwm = mapPWM(throttle, max_throttle_pwm, zero_throttle_pwm ,min_throttle_pwm);
      if (isSafeThrottle(throttle_pwm))
        analogWrite(escPin, throttle_pwm);
       else
        analogWrite(escPin, zero_throttle_pwm);
        
      //Serial.print("throttle set to ");
      //Serial.print(throttle);
    }
    else if (ste_pos >= 0){
      steering = input.substring(ste_pos + 9).toFloat();
      steering_pwm = mapPWM(steering, max_steering_pwm, center_steering_pwm, min_steering_pwm);
      if (isSafeSteering(steering_pwm))
        analogWrite(servoPin, steering_pwm);
       else
        analogWrite(servoPin, center_steering_pwm);
        
      //Serial.print("steering set to ");
      //Serial.print(steering);
    }
    else if (stringContains(input, "shutdown")){
      analogWrite(servoPin, center_steering_pwm);
      analogWrite(escPin, zero_throttle_pwm);
    }
  }

  else if (input.indexOf("try") >= 0){ //calibration process
    int thr_pos = input.indexOf("throttle");
    int ste_pos = input.indexOf("steering");
    if (thr_pos >= 0){
      throttle_pwm = input.substring(thr_pos + 9).toInt();
      analogWrite(escPin, throttle_pwm);
    }
    else if (ste_pos >= 0){
      steering_pwm = input.substring(ste_pos +9).toInt();
      analogWrite(servoPin, steering_pwm);
    }
  }

  else if (input.indexOf("calibrate") >= 0){
    if (stringContains(input, "maxforward"))
      max_throttle_pwm = getIntAfter(input, "_");

    else if (stringContains(input, "maxreverse"))
      min_throttle_pwm = getIntAfter(input, "_");

    else if (stringContains(input, "neutralthrottle"))
      zero_throttle_pwm = getIntAfter(input, "_");

    else if (stringContains(input, "maxright"))
      min_steering_pwm = getIntAfter(input, "_");

    else if (stringContains(input, "maxleft"))
      max_steering_pwm = getIntAfter(input, "_");

    else if (stringContains(input, "neutralsteering"))
      center_steering_pwm = getIntAfter(input, "_");
  }
}
