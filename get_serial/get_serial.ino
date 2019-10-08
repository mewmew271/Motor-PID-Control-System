int v = 0;
void setup() {
  Serial.begin(9600);
}

void loop() {
  v+=1;
  Serial.println(v);
  delay(500);
}
