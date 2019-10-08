import processing.serial.*;
Serial mySerial;
PrintWriter output;

void setup() {
   mySerial = new Serial( this, Serial.list()[0], 9600 );
   output = createWriter( "motor_output.csv" );
}

void draw() {
    if (mySerial.available() > 0 ) {
         String value = mySerial.readString();
         if ( value != null ) {
              output.println( value );
         }
    }
}

void keyPressed() {  // When a key is pressed
    output.flush();  // Write the remaining data to the file
    output.close();  // Finish the file
    exit();  // Stops the program
}
