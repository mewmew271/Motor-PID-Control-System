//sudo chmod a+rw /dev/ttyACM0

//Hard coded ratio, Number of interrupts per shaft rotation AFTER the gear box. It is in sense of number of CYCLES in single channel output.
#define ENC_COUNT_REV 4425.156

// Encoder output to Arduino Interrupt pin 2 and 3.
//Actually 1 interrupt only is needed.
#define encoder0PinA  2
#define encoder0PinB  3


// Numbers of pins for motor control
int PWMduty = 8; 
int HBridge1 = 9;
int HBridge2 = 10;

//Analog pin, used for reading the pot voltage
int SpeedControl1 = A0;

volatile long encoder0Pos=0;
long newposition;
long oldposition = 0;
unsigned long newtime;
unsigned long oldtime = 0;
float vel;
// Variable for RPM measuerment
float rpm = 0;

//PID errors and times
float ErrorRPM;
float last_ErrorRPM = 0;
float cumulative_error;
float rateError;
float PID;
//PID constants
double kp = 2
double ki = 5
double kd = 1

//Function run once on startup of a program. Place to initialize variables, pin mode and libraries.
void setup(){
  //Input pins for encoder chanels, Channel A must be an interrupt, and is connected to interrupt below
  pinMode(encoder0PinA, INPUT_PULLUP);
  pinMode(encoder0PinB, INPUT_PULLUP);

  //This line attaches interript to INt.0, which is pin 2
  attachInterrupt(0, positionCounter, RISING);

  //Engine pins
  //PWM for motor controller, pin 8.
  pinMode(PWMduty, OUTPUT);
  //Control inputs for HBridge. 10 for + direction 01 for - direction. Pins 9 and 10.
  pinMode(HBridge1, OUTPUT);
  pinMode(HBridge2, OUTPUT);

  //Serial interface/debug commands
  Serial.begin (9600);
  Serial.println("start");
}

void loop(){
  //Fetches position(integrated position from start) at the moment of the loop execution
  newposition = encoder0Pos;
  //Fetches time at the same moment
  newtime = millis();
  
  //Read voltage from the pot as an input, range 0-5V, mapped to 0-1023 by ADC
  int PotInput = analogRead(SpeedControl1);//range 0 1023
  //Translate volte input to RPM, it translates a signal form user to RPM unit
  float IdealRPM_float = voltageToRPM(PotInput);
  //Computes ABSOLUTE value of RPM
  float ActualRPM = velocityComputation();

  //Error calculation

  //Controller

  //Output to motors

  //THIS SHOULD BE DONE BASED ON PID OUTPUT NOT ON USER SIGNAL
  //THIS SHOULD BE DONE BASED ON PID OUTPUT NOT ON USER SIGNAL
  //THIS SHOULD BE DONE BASED ON PID OUTPUT NOT ON USER SIGNAL
  // Turn on motor A
  if (IdealRPM < 0) {
    digitalWrite(HBridge1, LOW);
    digitalWrite(HBridge2, HIGH);
  }
  else {
    digitalWrite(HBridge1, HIGH);
    digitalWrite(HBridge2, LOW);
  }
  
  // Set speed to 200 out of possible range ~127-255]
  int motor_pwm = map(abs(IdealRPM/100), 0, 23, 0, 255);
  analogWrite(PWMduty, motor_pwm);

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
  //Strore position used for this cycle
  oldposition = newposition;
  //Store time of fetching fir this cycle
  oldtime = newtime;
  delay(250);

  ///Leaving it out for a moment  
  //New PID stuff
  //elapsed_time = newtime - oldtime;
  //cumulative_error += ErrorRPM * elapsedTime;
  //rateError = (ErrorRPM - last_ErrorRPM)/elapsedTime;
  //PID = kp * ErrorRPM + ki * cumulative_error + kd * rateError;
}

//This is an interrupt triggered at all rising edges of Int.0(pin2)
void positionCounter()
{
  //Increase the position when both channels are high on interrupt
  if (digitalRead(encoder0PinA) == digitalRead(encoder0PinB)) {
    encoder0Pos++;
  } 
  //Decrease the position when both channels are high on interrupt
  else {
    encoder0Pos--;
  }
}

//This function maps input voltage to RPM value. This is goal setting signal for the loop.
float voltageToRPM(int inputVoltage)
{
  //factor of hundred thrown in, since map does not do float?
  int IdealRPM = map(inputVoltage, 0, 1023, -2300, 2300);
  return (float)IdealRPM/100;
}

//This functions differentiates position, effectively computing velocity
float velocityComputaion(){
   vel = (float)(abs(newposition-oldposition) * 1000) /(newtime-oldtime); 
  //fix the abs vel and the abs ActualRPM
  float ActualRPM = (float)(vel * 60 / ENC_COUNT_REV);
}
