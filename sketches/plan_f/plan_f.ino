/*
TODO: 
  PID Function - https://www.youtube.com/watch?v=CgJROEmC914
A Timer Speed Callback - https://playground.arduino.cc/Code/Timer/
J Combine Inputs
  Setup Gazebo Simulation - http://gazebosim.org/tutorials?cat=build_robot
  Layout Points for Essay

  sudo chmod a+rw /dev/ttyACM0
  368.763:1 gear ratio
*/


#include <timer.h>
#define ENC_COUNT_REV 4425.156


//timer 
auto timer = timer_create_default(); // create a timer with default settings

//PINS
const int encoder0PinA = 2;
const int encoder0PinB = 3;
const int enA = 6;  //PWM
const int in1 = 9;  //DIR1
const int in2 = 10; //DIR2

//HARDWARE ONLY
const bool HARDWARE_INPUT = false;
const int directionSwitch = 13; 
const int Potentiometer_in = A0;
const int POTENTIOMETER_SCALE = 1023;

//Global Variables
volatile long encoder0Pos = 0;
float actual_rpm = 1;
float encoder_rpm = 0;

//BRANCH: Input
int input() {

	String inString = "";
	while (Serial.available() > 0) { //While serial connection exists
		
		int inChar = Serial.read(); //Read char
		
		if (isDigit(inChar)) 
		{ //If char isnum
			inString += (char)inChar;
		}
		else if (inChar == 45)
		{ //If char is '-'
			inString += (char)inChar;
		}
		else 
		{ //Convert to int and return
			return inString.toInt(); //TODO: Convert toFloat()
		}
	}
}


//rpm calculations----------------------------------
volatile long rpm_counter = 0;
//variables to keep track of the timing of recent interrupts
unsigned long button_time = 0;
unsigned long last_button_time = 0;

bool Calculate_Actual_RPM(void *){
  //Serial.print(" |  RPS (" + (rpm_counter*10) + ")");
  //Serial.print(" |  RPM (" + (rpm_counter*10)*60 + ")");

  encoder_rpm = (rpm_counter*10)*60;
  actual_rpm = encoder_rpm/4425.156;//368.763;
  rpm_counter = 0; 
  
  return true; // keep timer active? true
 
}


//https://github.com/contrem/arduino-timer
//Called by the interrupt
//Increments the rpm_counter
void rpm_incrementor(){
  button_time = micros();//millis() //millisecconds 
  //check to see if increment() was called in the last 100 microseconds 3/1000000 seconds 
  if (button_time - last_button_time > 100)//may need to change //10,000 rpm max //TODO: Convert 100 to const thinggy
  {
    rpm_counter++;
    //Serial.print(" |  RPM_Count:" + rpm_counter);
    last_button_time = button_time;
  }
}




//BRANCH: Feedback

// Encoder output to Arduino Interrupt pin 2 and 3
// A - - A A - - A
// B B - - B B - -

// Update motor position
int newposition = 0;
int oldposition;
int newtime = 0;
int oldtime;

// Calculate speed
float Speed_Calculation() {
	newposition = encoder0Pos;
	newtime = millis();
	
	float position_difference =(float)(newposition - oldposition);
	//Serial.print ("P_Dif(" + String(position_difference) + ") | ");

	float time_difference = (float)(newtime - oldtime);
	//Serial.print ("T_Dif(" + String(time_difference ) + ") | ");
	
	float interupts_per_sec = (float)((position_difference * 1000)/time_difference);
	//Serial.print ("IPS(" + String(interupts_per_sec) + ") | ");

	float interupts_per_min = (float) interupts_per_sec * 60;
	//Serial.print ("IPM(" + String(interupts_per_min) + ") | ");

	float actual_rpm = (float)interupts_per_min / ENC_COUNT_REV;
	//Serial.print ("ACT(" + String(actual_rpm) + ") | ");
	
	oldposition = newposition;
	oldtime = newtime;
	
	return actual_rpm;
}






//BRANCH: Merged

//Calculate the rpm error
int previous_ideal_rpm;
float Error_Node(float ideal_rpm, float actual_rpm) {
	int rpm_error = previous_ideal_rpm - actual_rpm;
	previous_ideal_rpm = ideal_rpm;
	return rpm_error;
}


//Calculate PID Error
int pk = 1;
int ik = 1;
int dk = 1;
float PID_Controller(float ideal, float error) {

	//elapsed_time = newtime - oldtime;
	//cumulative_error += ErrorRPM * elapsedTime;
	//rateError = (ErrorRPM - last_ErrorRPM)/elapsedTime;

	return pk * ideal;// + ik * cumulative_error + dk * rateError;
}


//Enable motor
void Engine_Controller(int duty) {
	
	if (duty < 0) { //True if CLOCKWISE
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







void setup(){
	attachInterrupt(digitalPinToInterrupt(encoder0PinA), rpm_incrementor, FALLING);
  timer.every(100, Calculate_Actual_RPM);//100 mills 
	pinMode(enA, OUTPUT);
	pinMode(in1, OUTPUT);
	pinMode(in2, OUTPUT);
	Serial.begin (9600);
	Serial.println("start");
  
  //attachInterrupt(0, doEncoder, RISING); //check if this is doing anything as nothing is attached to pin interrupt pin 0
  //pinMode(directionSwitch, INPUT);
  //pinMode(encoder0PinA, INPUT_PULLUP);
  //pinMode(encoder0PinB, INPUT_PULLUP);
}

void loop() {
	timer.tick();
     //---------------------------
	//BRANCH: Input
	int ideal_rpm = input(); //TODO: convert to float
  
   
	//BRANCH: Feedback
	//float actual_rpm = Speed_Calculation(); //Moved assigbnment to the top of the script

	
	//BRANCH: Merged
	float error_rpm = Error_Node(ideal_rpm, actual_rpm);
	float duty = PID_Controller(ideal_rpm, error_rpm); //global: [pk, ik, dk]
	
	
	Engine_Controller(duty);

	Serial.print ("E_RPM," + String(encoder_rpm) + ", ");
	Serial.print ("RPM,(i:," + String(ideal_rpm) + ",|a:," + String(actual_rpm) + ",|e:," + String(error_rpm) + ",) | ,");
	Serial.print ("DUTY(," + String(duty) + ",) | ,");
	Serial.println();
	
}
