
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
// Direction Change
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






//sudo chmod a+rw /dev/ttyACM0
//368.763:1 gear ratio

#define ENC_COUNT_REV 4425.156
// Encoder output to Arduino Interrupt pin 2 and 3
const int encoder0PinA = 2;
const int encoder0PinB = 3;

// Motor A
const int enA = 8; //enA=PWM
const int in1 = 9; //DIR1
const int in2 = 10;//DIR2

// Potentiometer
const int Potentiometer_in = A0;


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


  // New PID stuff
  elapsed_time = newtime - oldtime;
  cumulative_error += ErrorRPM * elapsedTime;
  rateError = (ErrorRPM - last_ErrorRPM)/elapsedTime;
  PID = kp * ErrorRPM + ki * cumulative_error + kd * rateError;
  
  
// Calculate RPM
  float RPM_data[3]; // (Ideal, Actual, Error)
  vel = (float)(abs(newposition-oldposition) * 1000) /(newtime-oldtime);
  CalculateRPM(analogRead(Potentiometer_in), velocity, &RPM_data);


// Calculate Direction
  bool Direction = DIR(newPosition, oldPosition);

  
// Turn on motor A
  Engine_Controller(RPM_data[0], Direction)


//Log Data
  Serial.print ("Clockwise("+IdealDirection+") | ");
  Serial.println ("RPM(i:"+String(RPM_data[0])+"|a:"+String(RPM_data[1])+"|e:"+String(RPM_data[2])+") | ");


//Update Prior States
  Position_Counter();
}



void loop2() {

  //Branch A
  ideal_rpm = RPM_Mapping(Pot_Value)

  //Branch B
  Position_Counter(previous_position)
  actual_rpm = Speed Calculation()
  

  //Branch AB
  error_rpm = Error_Node(ideal_rpm, actual_rpm)
  duty = PID_ Controller() //global: [pk, ik, dk]
  polarity = Direction_Controller()
  Engine_Controller(duty, polarity)
  
}











void Engine_Controller(int duty, bool polarity) {
  if (dir) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }
  else {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  }
  
  // Set speed to X out of possible range [~127-255]
  int motor_pwm = map(rpm, 0, 23, 0, 255);
  analogWrite(enA, motor_pwm);
}


int POTENTIOMETER_SCALE = 1023;
void CalculateRPM(int potInput, float velocity, float *RPM_data)
{
 //Ideal
 RPM_data[0] = (float) abs(map(potInput, 0, POTENTIOMETER_SCALE, -2300, 2300)/100);
 
 //Actual
 RPM_data[1] = (float) abs(velocity * 60 / ENC_COUNT_REV));

 //Error
 RPM_data[2] = (float) abs(abs(RPM_data[0]) - abs(RPM_data[1]));
}


bool DIR(int newPos, int oldPos) {

  // Calculate Direction
  if (newPos > oldPos){
    return false;
  }
  return true;
}


// Update current position of motor.
void Position_Counter() {
  oldposition = newposition;
  oldtime = newtime;
  delay(250);
}
