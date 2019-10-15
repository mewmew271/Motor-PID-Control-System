//sudo chmod a+rw /dev/ttyACM0
//368.763:1 gear ratio

#define ENC_COUNT_REV 4425.156

/*
  void loop2() {

  //Branch A
  ideal_rpm = RPM_Mapping(Pot_Value)

  //Branch B
  Position_Counter(previous_position)
  actual_rpm = Speed_Calculation()


  //Branch AB
  error_rpm = Error_Node(ideal_rpm, actual_rpm)
  duty = PID_ Controller() //global: [pk, ik, dk]
  polarity = Direction_Controller()
  Engine_Controller(duty, polarity)

  }
*/





// Encoder output to Arduino Interrupt pin 2 and 3
const int encoder0PinA = 2;
const int encoder0PinB = 3;
volatile long encoder0Pos = 0;
void doEncoder()
{
  if (digitalRead(encoder0PinA) == digitalRead(encoder0PinB)) {
    encoder0Pos++;
  } else {
    encoder0Pos--;
  }
}



//BRANCH: Input

//Define direction
const int directionSwitch = 13; 
bool Direction_Controller() {
  return digitalRead(directionSwitch);
}


//Map Potentiometer to Usable Scale (Increase by turning AntiClockwise)
int POTENTIOMETER_SCALE = 1023;
float RPM_Mapping(int potInput, bool polarity)
{
  float ideal_rpm = (float) map(potInput, 0, POTENTIOMETER_SCALE, 0,2300) / 100;
  if (polarity) {
    ideal_rpm  = -ideal_rpm;
  }
  return ideal_rpm;
}





//BRANCH: Feedback

// Update motor position
int newposition = 0;
int oldposition;
int newtime = 0;
int oldtime;
/*
void Position_Counter() {
  oldposition = newposition;
  oldtime = newtime;
  delay(250);
}
*/

// Calculate speed
float Speed_Calculation() {
  newposition = encoder0Pos;
  newtime = millis();
  
  float position_difference =(float)(newposition - oldposition);
  Serial.print ("P_Dif(" + String(position_difference) + ") | ");

  float time_difference = (float)(newtime - oldtime);
  Serial.print ("T_Dif(" + String(time_difference ) + ") | ");
  
  float interupts_per_sec = (float)((position_difference * 1000)/time_difference);
  //Serial.print ("IPS(" + String(interupts_per_sec) + ") | ");

  float interupts_per_min = (float) interupts_per_sec * 60;
  //Serial.print ("IPM(" + String(interupts_per_min) + ") | ");

  float actual_rpm = (float)interupts_per_min / ENC_COUNT_REV;
  Serial.print ("ACT(" + String(actual_rpm) + ") | ");
  
  oldposition = newposition;
  oldtime = newtime;
  
  return actual_rpm;
}






//BRANCH: Merged

//Calculate the rpm error
float Error_Node(float ideal_rpm, float actual_rpm) {
  return (ideal_rpm - actual_rpm);
}


//Calculate PID Error
int pk = 1;
int ik = 1;
int dk = 1;
float PID_Controller(float error) {

  //elapsed_time = newtime - oldtime;
  //cumulative_error += ErrorRPM * elapsedTime;
  //rateError = (ErrorRPM - last_ErrorRPM)/elapsedTime;

  return pk * error;// + ik * cumulative_error + dk * rateError;
}


//Enable motor
const int enA = 6; //enA=PWM
const int in1 = 9; //DIR1
const int in2 = 10;//DIR2
void Engine_Controller(int duty, bool polarity) {
  
  if (polarity) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }
  else {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  }
  
  // Set speed to X out of possible range [~127-255]
  int motor_pwm = map(abs(duty), 0, 23, 0, 255);
  analogWrite(enA, motor_pwm);
}














const int Potentiometer_in = A0;
void setup(){
  
  pinMode(encoder0PinA, INPUT_PULLUP);
  pinMode(encoder0PinB, INPUT_PULLUP);
  pinMode(directionSwitch, INPUT);
  attachInterrupt(0, doEncoder, RISING); 
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  Serial.begin (9600);
  Serial.println("start");
}
void loop() {
  
  //BRANCH: Input
  bool polarity = Direction_Controller();
  Serial.print ("DIR(" + String(polarity) + ") | ");

  float ideal_rpm = RPM_Mapping(analogRead(Potentiometer_in), polarity);
  Serial.print ("POT(" + String(analogRead(Potentiometer_in)) + ") | ");
  Serial.print ("IRPM(" + String(ideal_rpm) + ") || ");


  
  //BRANCH: Feedback
  float actual_rpm = Speed_Calculation();

  
  //BRANCH: Merged
  //float error_rpm = Error_Node(ideal_rpm, actual_rpm);
  //float duty = PID_Controller(error_rpm); //global: [pk, ik, dk]
  
  
  Engine_Controller(ideal_rpm, polarity);

  
  //Serial.print ("RPM(i:" + String(ideal_rpm) + "|a:" + String(actual_rpm) + "|e:" + String(error_rpm) + ") | ");
  //Serial.print ("DUTY(" + String(duty) + ") | ");
  Serial.println();


  delay(250);
}
