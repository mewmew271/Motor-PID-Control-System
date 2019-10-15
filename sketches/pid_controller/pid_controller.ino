// define some constants
int ActPos = A0;    // select the input pin for feedback signal
int DesPos = A1;    // select the input pin for control signal

byte PWMOutput;
long Error[10];
long Accumulator;
long PID;
int PTerm;
int ITerm;
int DTerm;
//byte Divider;

/* GetError():
Read the analog values, shift the Error array down 
one spot, and load the new error value into the
top of array.
*/
void GetError(void)
{
  byte i = 0;
  /*
   * MAYBE WE NEED TO CHANGE 
   * THE VARIABLES TO INT OR FLOAT
   */

  // read analog signals
  word ActualPosition = analogRead(ActPos);  
  // comment out to speed up PID loop
  //  Serial.print("ActPos= ");
  //  Serial.println(ActualPosition,DEC);

  word DesiredPosition = analogRead(DesPos);
  // comment out to speed up PID loop
  //  Serial.print("DesPos= ");
  //  Serial.println(DesiredPosition,DEC);

  // shift error values
  for(i=9;i>0;i--)
    Error[i] = Error[i-1];
  // load new error into top array spot  
  
  Error[0] = (long)DesiredPosition-(long)ActualPosition;
  // comment out to speed up PID loop
  //  Serial.print("Error= ");
  //  Serial.println(Error[0],DEC);
}

/* CalculatePID():
Error[0] is used for latest error, Error[9] with the DTERM
*/
void CalculatePID(void)
{
// Set constants here
  PTerm = 1;
  ITerm = 1;
  DTerm = 0;
  //Divider = 10;

  // Calculate the PID  
  PID = Error[0]*PTerm;     // start with proportional gain
  Accumulator += Error[0];  // accumulator is sum of errors
  PID += ITerm*Accumulator; // add integral gain and error accumulation
  PID += DTerm*(Error[0]-Error[9]); // differential gain comes next
  PID = PID//>>Divider; // scale PID down with divider

  // comment out to speed up PID loop  
  //Serial.print("PID= ");
  //  Serial.println(PID,DEC);

  // limit the PID to the resolution we have for the PWM variable
  if(PID>=127)
    PID = 127;
  if(PID<=-126)
    PID = -126;

  //PWM output should be between 1 and 254 so we add to the PID    
  PWMOutput = PID + 127;

  // comment out to speed up PID loop
  //  Serial.print("PWMOutput= ");
  //  Serial.println(PWMOutput,DEC);

}
