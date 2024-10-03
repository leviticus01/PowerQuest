#define HALL 16

void setup() {
  Serial.begin(9600);

  pinMode(HALL,INPUT);
  // put your setup code here, to run once:

}

void loop() {

  int d = digitalRead(HALL);
  Serial.print("d = ");
  Serial.println(d);
  delay(100);
  
/*
  int a = analogRead(HALL);
  Serial.print("a = ");
  Serial.println(a);
  delay(500);
*/
}
