/*

*/

// ESP32 PIN DEFINITIONS
#define SD_DO         0
#define SD_DI         1
#define IO_MOTION     2 // Motion sensor
#define IO_LIGHT      3 // Light peripheral
#define AMP_DIN       4
#define AMP_BCLK      5
#define AMP_LRC       6
#define SD_CLK        7
#define SD_CS         8
#define IO_SEL1       10 // Peripheral 1 selected with switch
#define IO_SEL2       11 // Peripheral 2 selected with switch (technically unused, can be applied elsewhere if needed)
#define IO_HALL       15 // Hall sensor
#define IO_PATH3      18 // LED path 3
#define IO_PATH2      19 // LED path 2
#define IO_PATH1      20 // LED path 1
#define IO_POT        21 // Motor speed potentiometer
#define MOTOR_2       22 // PWM motor driver
#define MOTOR_1       23 // PWM motor driver

// VARIABLES
bool hall_state_curr = 0; // 1 if battery is in place (hall sensor active)
bool hall_state_prev = 0; // 1 if battery is in place (hall sensor active)

// Runs Once, required
void setup() {
  
  // INPUTS
  pinMode(IO_POT, INPUT);
  pinMode(IO_MOTION, INPUT);
  pinMode(IO_SEL1, INPUT);
  pinMode(IO_SEL2, INPUT);
  pinMode(IO_HALL, INPUT);

  // OUTPUTS
  pinMode(IO_LIGHT, OUTPUT);
  pinMode(IO_PATH3, OUTPUT);
  pinMode(IO_PATH2, OUTPUT);
  pinMode(IO_PATH1, OUTPUT);

  //zzztodo need to set pin modes for sd, amp, and motor driver
}

// Runs perpetually
void loop() {

  // always check if the hall sensor is active
  check_hall();
  
  // only run this code while the battery is inserted
  if (hall_state_curr == 1)
  {
    bool p1_selected = digitalRead(IO_SEL1); // check the selector switch
    digitalWrite(IO_PATH3, HIGH); // turn on the path from the battery to the switch

    // peripheral 1 code (light)
    if(p1_selected == 1) {
      
      // update paths
      digitalWrite(IO_PATH1, HIGH);
      digitalWrite(IO_PATH2, LOW);

      // motion/light peripheral functionality
      check_motion();

    }

    // peripheral 2 code (motor)
    else {

      // update paths
      digitalWrite(IO_PATH2, HIGH);
      digitalWrite(IO_PATH1, LOW);

      run_motor();

    }

  }
  
  // when the battery is removed, turn things off
  else
  {
    digitalWrite(IO_PATH1, LOW);
    digitalWrite(IO_PATH2, LOW);
    digitalWrite(IO_PATH3, LOW);
  }
}

// Checks if the hall sensor state has changed
void check_hall() {
  hall_state_curr = digitalRead(IO_HALL); // check current battery state

  // if the hall sensor goes from inactive to active
  if ((hall_state_curr == 1) && (hall_state_prev == 0)) {

    // zzztodo what happens if the battery is inserted? play audio?
    play_audio();
  }

  hall_state_prev = hall_state_curr; // make the current battery state the previous state
}

// Plays audio from SD card
void play_audio() {
  // zzztodo
}

// motion sensor and light peripheral 
void check_motion() {
      bool motion_sensed = digitalRead(IO_MOTION);
      if (motion_sensed == 1) {
        digitalWrite(IO_LIGHT, HIGH);
      }
      else
      {
        digitalWrite(IO_LIGHT, LOW);
      }
}

// motor peripheral
void run_motor() {
  // zzztodo
}
