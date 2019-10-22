
String inString = "";		// string to hold input
int test = 0;

//speed controller
const int enA = 6; //enA=PWM
const int in1 = 9; //DIR1
const int in2 = 10;//DIR2

//hall effect sensors
const int encoder0PinA = 2;
const int encoder0PinB = 3;

int lastState = 0;


void setup() {
	// Open serial communications and wait for port to open:
	Serial.begin(9600);
	while (!Serial) {
		; // wait for serial port to connect. Needed for native USB port only
	}

	// send an intro:
	Serial.println("\n\nString toInt():");
	Serial.println();
}

void loop() {
	int motor_pwm = 0;
	analogWrite(enA, motor_pwm);
	// Read serial input:
	while (Serial.available() > 0) {
		int inChar = Serial.read();
		Serial.println(inChar);
		if (isDigit(inChar)) {
			// convert the incoming byte to a char and add it to the string:
			inString += (char)inChar;
			Serial.println("added");
		}
		else if (inChar == 45)//45 aski is -
		{ //reverce
			digitalWrite(in1, LOW);
			digitalWrite(in2, HIGH);
		}
		else {
			digitalWrite(in1, HIGH);
			digitalWrite(in2, LOW);
		}
	

		// if you get a newline, print the string, then the string's value:
		if (inChar == '\n') {
			if (test == 0 ) {//if test is 0 set test to wanted position
				test = inString.toInt();
			}
			Serial.print("Value:");//print value set too
			Serial.println(inString.toInt());

			while (test != 0 ) {
				int motor_pwm = 200;
				analogWrite(enA, motor_pwm);

				Serial.print("positions left to go: ");
				Serial.println(test);


				//	-------------------------------

				int state = digitalRead(encoder0PinA);

				if (state == HIGH && lastState == 0) {
					lastState = 1;
					test--;
				}
				if (state == LOW && lastState == 1) {
					lastState = 0;

				}
			}
			// -------------------
		}
		// clear the string for new input:
		inString = "";
	}
}
