
const int directionSwitch = 13; 

void setup(){
 
  pinMode(directionSwitch, INPUT);
 
}

void loop(){

  motorDirection = analogRead(directionSwitch) 


  if (motorDirection = HIGH){
    digitalWrite(HBridge1, LOW);
    digitalWrite(HBridge2, HIGH);
    }
    else 
    {
    digitalWrite(HBridge1, HIGH);
    digitalWrite(HBridge2, LOW);
    }


}
