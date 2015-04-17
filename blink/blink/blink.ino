int ledje = 13;
int pause = 250;



void setup() {
  // put your setup code here, to run once:
 pinMode(ledje, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
 
  digitalWrite(ledje, HIGH);
  delay(pause);
  digitalWrite(ledje, LOW);
  delay(pause*4);
}
