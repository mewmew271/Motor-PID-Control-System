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
const int encoder0PinA = 2; //Channel A
const int encoder0PinB = 3; //Channel B
const int enA = 6;  //PWM
const int in1 = 9;  //DIR1
const int in2 = 10; //DIR2

//Global Variables
volatile long encoder0Pos = 0;

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
    else if (inChar == 46)
    { //If char is '.'
      inString += (char)inChar;
      //TODO: Check what happens for input "14."
    }
		else 
		{
      //Convert to float, handle edge cases, return value.
      float xxx = inString.toFloat();
      if (xxx > 23) {
        xxx = 23;
      }
      else if (xxx < -23) {
        xxx = -23;
		  }
			return xxx;
		}
	} 
}


//rpm calculations----------------------------------
volatile long A_count_per_ms = 0; //Encoder triggers/ms
float triggers_pm = 0;
float encoder_rpm = 0;
float actual_rpm = 0;

//Track timings of recent Interrupts
unsigned int update_buffer_ms = 200;
unsigned long A_button_time = 0;
unsigned long A_last_button_time = 0;
bool actual_polarity = 0;


bool Calculate_Actual_RPM(void *){

  triggers_pm = (A_count_per_ms*10)*60;
  encoder_rpm = (triggers_pm/48);
  actual_rpm =  encoder_rpm / ENC_COUNT_REV;

  if (actual_polarity) {
    actual_rpm = -actual_rpm;  
  }
  
  A_count_per_ms = 0;
  
  return true; // keep timer active? true
}


//https://github.com/contrem/arduino-timer
//Called by the Interrupt
void A_Channel_Incrementor(){
  A_button_time = micros();//millis() //millisecconds 
  
  //check to see if increment() was called in the last 100 microseconds 3/1000000 seconds 
  if (A_button_time - A_last_button_time > update_buffer_ms)//may need to change //10,000 rpm max //TODO: Convert 100 to const thinggy
  {
    A_count_per_ms++;
    A_last_button_time = A_button_time;
  }


  //On falling edge of channel A, use the state of B to determine direction.
  if (digitalRead(encoder0PinB)) {
    actual_polarity = 0; //0 defines clockwise
  }
  else {
    actual_polarity = 1;
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
  attachInterrupt(digitalPinToInterrupt(encoder0PinA), A_Channel_Incrementor, FALLING);
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

	//BRANCH: Input
	int ideal_rpm = input();
  
   
	//BRANCH: Feedback
	//float actual_rpm = Speed_Calculation(); //Moved assignment to the top of the script

	
	//BRANCH: Merged
	float error_rpm = Error_Node(ideal_rpm, actual_rpm);
	float duty = PID_Controller(ideal_rpm, error_rpm); //global: [pk, ik, dk]
	
	
	Engine_Controller(duty);

	Serial.print ("E_RPM," + String(encoder_rpm) + " ,");
	Serial.print ("RPM(i:," + String(ideal_rpm) + ",|a:," + String(actual_rpm) + ",|e:," + String(error_rpm) + ",) | ,");
	Serial.print ("DUTY(," + String(duty) + ",) | ,");
	Serial.println();
	
}
