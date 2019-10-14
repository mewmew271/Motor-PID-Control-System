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
void loop() {

  //Branch A
  ideal_rpm = RPM_Mapping(analogRead(Potentiometer_in))

  //Branch B
  Position_Counter(previous_position)
  actual_rpm = Speed_Calculation()
  

  //Branch AB
  error_rpm = Error_Node(ideal_rpm, actual_rpm)
  duty = PID_Controller() //global: [pk, ik, dk]
  polarity = Direction_Controller()
  Engine_Controller(duty, polarity)
  
}


// Encoder output to Arduino Interrupt pin 2 and 3
const int encoder0PinA = 2;
const int encoder0PinB = 3;
volatile long encoder0Pos=0;
void doEncoder()
{
  if (digitalRead(encoder0PinA) == digitalRead(encoder0PinB)) {
    encoder0Pos++;
  } else {
    encoder0Pos--;
  }
}




//BRANCH: Input

//Map Potentiometer to Usable Scale
int POTENTIOMETER_SCALE = 1023;
float RPM_Mapping(int potInput)
{
 return (float) abs(map(potInput, 0, POTENTIOMETER_SCALE, -2300, 2300)/100);
}





//BRANCH: Feedback

// Update motor position
int newposition = 0;
int oldposition;
int newtime = 0;
int oldtime;
void Position_Counter() {
  oldposition = newposition;
  oldtime = newtime;
  delay(250);
  newposition = encoder0Pos;
  newtime = millis();
}


// Calculate speed
float Speed_Calculation() {
  float velocity = (float)(abs(newposition-oldposition) * 1000)/(newtime-oldtime);
  return = (float) abs(velocity * 60 / ENC_COUNT_REV));
}






//BRANCH: Merged

//Calculate the rpm error
float Error_Node(float ideal_rpm, float actual_rpm) {
  return (ideal_rpm-actual_rpm)
}


//Calculate PID Error
int pk = 1;
int ik = 1;
int dk = 1;
float PID_Controller(float error) {
  
  //elapsed_time = newtime - oldtime;
  //cumulative_error += ErrorRPM * elapsedTime;
  //rateError = (ErrorRPM - last_ErrorRPM)/elapsedTime;
  
  return pk * ErrorRPM;// + ik * cumulative_error + dk * rateError;
}

//Determine direction
bool Direction_Controller() {
  // Calculate Direction
  if (newPosition > oldPosition) {
    return false;
  }
  return true;
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
  int motor_pwm = map(duty, 0, 23, 0, 255);
  analogWrite(enA, motor_pwm);
}
