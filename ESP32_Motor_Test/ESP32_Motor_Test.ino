#define IO_POT        A0 // Motor speed potentiometer
#define MOTOR_2       22 // PWM motor driver
#define MOTOR_1       21 // PWM motor driver
#define FAULT         1 // Fault pin

//const int io_pot = A0;

void setup() {
  // put your setup code here, to run once:
  pinMode(IO_POT,INPUT);
  pinMode(MOTOR_1,OUTPUT);
  pinMode(MOTOR_2,OUTPUT);
  pinMode(FAULT,INPUT);

  Serial.begin(115200);

}

void loop() {
  int pot_raw = analogRead(IO_POT);

  int pwm_rate = map(pot_raw,0,3500,0,255);
  if(pwm_rate < 0)
  {
    pwm_rate = 0 ;
  }
  if(pwm_rate > 255) {
    pwm_rate = 255 ;
  }
  Serial.println(pwm_rate);
  //if(digitalRead(FAULT) == LOW) {
  //  Serial.println("Fault Detected!");
  //} else {
  //  Serial.println("No Fault.");
  //}
  //Serial.println("===============");
  //delay(500);
  
  analogWrite(MOTOR_1,pwm_rate);
  digitalWrite(MOTOR_2,LOW);

}
