//sudo chmod a+rw /dev/ttyACM0

#define ENC_COUNT_REV 4425.156
// Encoder output to Arduino Interrupt pin 2 and 3
const int encoder0PinA = 2;
const int encoder0PinB = 3;
// Motor A
const int enA = 8; //enA=PWM
const int in1 = 9; //DIR1
const int in2 = 10;//DIR2
//pot 
const int SpeedControl1 = A0;


volatile long encoder0Pos=0;

//Position Variables
long newposition;
long oldposition = 0;

//Time Variables
unsigned long newtime;
unsigned long oldtime = 0;

//Speed Variables
float vel;
float rpm;

//PID errors and times
float ErrorRPM;
float last_ErrorRPM = 0;
float cumulative_error;
float rateError;
float PID;

//PID constants
double kp = 1;
double ki = 1;
double kd = 1;

void doEncoder()
{
  if (digitalRead(encoder0PinA) == digitalRead(encoder0PinB)) {
    encoder0Pos++;
  } else {
    encoder0Pos--;
  }
}

/*
//New encoder version. Might work better. 
int encoder0PinALast = LOW;

void doEncoderV2(){
  n = digitalRead(encoder0PinA);
  if ((encoder0PinALast == LOW) && (n == HIGH)) {
    if (digitalRead(encoder0PinB) == LOW) {
      encoder0Pos--;
    } 
    else {
      encoder0Pos++;
    }
  }
  encoder0PinALast = n;
}
*/

/* Debounce function
boolean debounceButton(boolean state){
  boolean stateNow = digitalRead(button);
  if(state!=stateNow)
  {
    delay(20);
    stateNow = digitalRead(button);
  }
  return stateNow;  
}
*/

/*
//Direction Change
int rotDirection = 0;
boolean pressed = LOW;

//MOVE TO void setup()
//pinMode(button, INPUT); 
//Set initial rotation direction
//digitalWrite(in1, LOW);
//digitalWrite(in2, HIGH);

void dirChange(){
  //debounceButton Function
  pressed = debounceButton(button);
  // If button is pressed - change rotation direction
  if (pressed == true  & rotDirection == 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    rotDirection = 1;
    delay(20);
  }
  // If button is pressed - change rotation direction
  if (pressed == false & rotDirection == 1) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    rotDirection = 0;
    delay(20);
  }
}
*/

void setup(){
  
  pinMode(encoder0PinA, INPUT_PULLUP);
  pinMode(encoder0PinB, INPUT_PULLUP);
  
  attachInterrupt(0, doEncoder, RISING); 
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  Serial.begin (9600);
  Serial.println("start");
}

void loop(){
  newposition = encoder0Pos;
  newtime = millis();
  
  //New PID stuff
  elapsed_time = newtime - oldtime;
  cumulative_error += ErrorRPM * elapsedTime;
  rateError = (ErrorRPM - last_ErrorRPM)/elapsedTime;
  PID = kp * ErrorRPM + ki * cumulative_error + kd * rateError;
  
  // Calculate Ideal/Actual/Error RPM
  int PotInput = analogRead(SpeedControl1);//range 0 1023
  int IdealRPM = map(PotInput, 0, 1023, -2300, 2300);
  float IdealRPM_float = (float)IdealRPM/100;
  
  vel = (float)(abs(newposition-oldposition) * 1000) /(newtime-oldtime);
  
  //fix the abs vel and the abs ActualRPM
  float ActualRPM = (float)(vel * 60 / ENC_COUNT_REV);
  
  // Turn on motor A
  if (IdealRPM < 0) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }
  else {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  }
  
  // Set speed to 200 out of possible range ~127-255]
  int motor_pwm = map(abs(IdealRPM/100), 0, 23, 0, 255);
  analogWrite(enA, motor_pwm);

  // Calculate Ideal/Actual/Error Direction
  String ActualDirection = "+";
  if (newposition > oldposition){ ActualDirection = "-"; ActualRPM=-ActualRPM;}
  String IdealDirection = "-";
  if (IdealRPM > 0){ IdealDirection = "+"; }
  
  float ErrorRPM = abs(IdealRPM_float - ActualRPM);

  //Log Data
  Serial.print ("T(Ideal|Actual) |");
  Serial.print ("Direction("+IdealDirection+"|"+ActualDirection+") | ");
  Serial.println ("RPM("+String(IdealRPM_float)+"|"+String(ActualRPM)+"|e:"+String(ErrorRPM)+") | ");

  //Update Prior States
  oldposition = newposition;
  oldtime = newtime;
  delay(250);
}
