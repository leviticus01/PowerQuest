// PowerQuest Main Program
// Developed by Levi Tucker & Jamya Mitchell
// ECE 3011
// FALL 2024

#include <driver/i2s.h>
#include <math.h>

// ESP32 PIN DEFINITIONS
#define IO_POT        A1  // Motor speed potentiometer (MOVED FROM PIN 21)
#define D_GND_1       1 // Digital ground pin for convenience
#define IO_MOTION     2 // Motion sensor
#define IO_LIGHT      3 // Light peripheral (white led)
#define I2S_DOUT      4 // Data to I2S amp
#define I2S_BCLK      5 // BCLK to I2S amp
#define I2S_LRCLK     6 // LRC to I2S amp
#define SD_CLK        7 // UNUSED
#define D_3V3_8       8 // 3.3V power for convenience
#define IO_SEL1       10 // Peripheral 1 selected with switch
#define D_GND_11      11 // Digital ground pin for convenience
#define IO_HALL       15 // Hall sensor
#define IO_PATH3      18 // LED path 3 (from battery to switch)
#define IO_PATH2      19 // LED path 2 (from switch to P1)
#define IO_PATH1      20 // LED path 1 (from switch to P2)
#define MOTOR_2       22 // PWM motor driver
#define MOTOR_1       23 // PWM motor driver

// I2S AUDIO DEFINITIONS
#define I2S_NUM       I2S_NUM_0
#define SAMPLE_RATE   44100

// VARIABLES (for detecting state changes)
bool hall_state_curr = 0; // 1 if battery is in place (hall sensor active)
bool hall_state_prev = 0; // 1 if battery is in place (hall sensor active)
bool switch_state_prev = 0;

// Runs Once, required
void setup() {
  
  // Serial for debugging
  //Serial.begin(115200);

  // I2S configuration
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S_MSB,
    .intr_alloc_flags = 0, // Default interrupt priority
    .dma_buf_count = 16,
    .dma_buf_len = 256,
    .use_apll = true,
    .tx_desc_auto_clear = true // Clears tx descriptor on underflow
  };

  // Pin configuration for I2S
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCLK,
    .ws_io_num = I2S_LRCLK,
    .data_out_num = I2S_DOUT,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  // Install and start I2S driver
  i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM, &pin_config);
  i2s_zero_dma_buffer(I2S_NUM);

  // Set input pin modes
  pinMode(IO_POT, INPUT);
  pinMode(IO_MOTION, INPUT);
  pinMode(IO_SEL1, INPUT);
  pinMode(IO_HALL, INPUT);

  // Set output pin modes
  pinMode(IO_LIGHT, OUTPUT);
  pinMode(IO_PATH3, OUTPUT);
  pinMode(IO_PATH2, OUTPUT);
  pinMode(IO_PATH1, OUTPUT);
  pinMode(MOTOR_1, OUTPUT);
  pinMode(MOTOR_2, OUTPUT);

  // Set digital ground pins
  pinMode(D_GND_11, OUTPUT);
  digitalWrite(D_GND_11, LOW);

  digitalWrite(IO_LIGHT, LOW);
  analogWrite(MOTOR_1, 0);
  digitalWrite(MOTOR_2, LOW);
  digitalWrite(IO_PATH1, LOW);
  digitalWrite(IO_PATH2, LOW);
  digitalWrite(IO_PATH3, LOW);

  switch_state_prev = digitalRead(IO_SEL1);

}

// MAIN LOOP
void loop() {


  
  // only run this code while the battery is inserted
  if (hall_state_curr == 1) {
    // for debugging
    //Serial.println("Hall ON");

    bool switch_state_curr = digitalRead(IO_SEL1); // check the selector switch
    digitalWrite(IO_PATH3, HIGH); // turn on the path from the battery to the switch

    // peripheral 1 code (light)
    if(switch_state_curr == 1) {
      //Serial.println("Light Selected");

      // update LED paths, turn off motor peripheral
      analogWrite(MOTOR_1, 0);
      digitalWrite(MOTOR_2, LOW);
      digitalWrite(IO_PATH1, HIGH);
      digitalWrite(IO_PATH2, LOW);

      // plays audio on switch change
      if(switch_state_prev == 0) {
        playAudio_rand();
        switch_state_prev = switch_state_curr;
      }



      // motion & light peripheral functionality
      light_peripheral();

    }

    // peripheral 2 code (motor)
    else {
      // for debugging
      //Serial.println("motor Selected");

      // update LED paths, turn off light peripheral
      digitalWrite(IO_PATH2, HIGH);
      digitalWrite(IO_PATH1, LOW);
      digitalWrite(IO_LIGHT, LOW);

      // plays audio on switch change
      if(switch_state_prev == 1) {
        playAudio_rand();
        switch_state_prev = switch_state_curr;
      }

      // motor peripheral functionality
      motor_peripheral();
    }

  }
  
  // when the battery is removed, turn everything off
  else
  {
    // for debugging
    //Serial.println("Hall OFF");
    digitalWrite(IO_LIGHT, LOW);
    analogWrite(MOTOR_1, 0);
    digitalWrite(MOTOR_2, LOW);
    digitalWrite(IO_PATH1, LOW);
    digitalWrite(IO_PATH2, LOW);
    digitalWrite(IO_PATH3, LOW);
  }

  // always check if the hall sensor is active
  check_hall();
}

// Checks if the hall sensor state has changed
void check_hall() {
  hall_state_curr = !digitalRead(IO_HALL); // check current battery state

  // if the hall sensor goes from inactive to active
  if ((hall_state_curr == 1) && (hall_state_prev == 0)) {

    digitalWrite(IO_PATH3, HIGH);
    playAudio_arp();
  }

  hall_state_prev = hall_state_curr; // make the current battery state the previous state
}

// motion sensor and light peripheral 
void light_peripheral() {
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
void motor_peripheral() {
  // get raw potentiometer data and map it to the PWM rate
  int pot_raw = analogRead(IO_POT);
  int pwm_rate = map(pot_raw,0,3500,0,255);
  Serial.println(pwm_rate);

  // limit minimum pwm rate to 0
  if(pwm_rate < 0) {
    pwm_rate = 0;
  }
  
  // limit maximum pwm rate to 255
  if(pwm_rate > 255) {
    pwm_rate = 255;
  }

  // turn on the motor!
  analogWrite(MOTOR_1,pwm_rate);
  digitalWrite(MOTOR_2, LOW);
}

// Plays Audio Tone (helper function for 'playAudio_*' functions)
void playTone(float frequency, int duration, float amplitude = 3000.0) {
  int numSamples = (SAMPLE_RATE * duration) / 1000;
  int16_t* buffer = new int16_t[numSamples];

  for (int i = 0; i < numSamples; i++) {
    float currentAmplitude = amplitude;
    // Fade-in for the first 10% of samples
    if (i < numSamples * 0.1) {
      currentAmplitude *= (float)i / (numSamples * 0.1);
    }
    // Fade-out for the last 10% of samples
    else if (i > numSamples * 0.9) {
      currentAmplitude *= (float)(numSamples - i) / (numSamples * 0.1);
    }
    buffer[i] = currentAmplitude * sin(2 * PI * frequency * i / SAMPLE_RATE);
  }

  size_t bytes_written;
  i2s_write(I2S_NUM, buffer, sizeof(int16_t) * numSamples, &bytes_written, portMAX_DELAY);
  delete[] buffer;
}

// AUDIO MELODIES
//===============================================================================//
// Variation 1: Sine wave melody
void playAudio_sine() {
  const float notes[] = {261.63, 329.63, 392.00, 523.25}; // C4, E4, G4, C5
  const int durations[] = {200, 200, 200, 400};
  for (int i = 0; i < 4; i++) {
    playTone(notes[i], durations[i]);
    delay(50);
  }
}

// Variation 2: Ascending and Descending Arpeggio
void playAudio_arp() {
  const float notes[] = {261.63, 329.63, 392.00, 523.25, 392.00, 329.63, 261.63};
  const int durations[] = {150, 150, 150, 300, 150, 150, 300};
  for (int i = 0; i < 7; i++) {
    playTone(notes[i], durations[i]);
    delay(50);
  }
}

// Variation 3: Random Frequencies for a Futuristic Sound
void playAudio_rand() {
  for (int i = 0; i < 8; i++) {
    float randomFreq = random(200, 800); // Random frequency between 200Hz and 800Hz
    playTone(randomFreq, 200);
    delay(30);
  }
}

// Variation 4: Minor Chord Melody
void playAudio_minor() {
  const float notes[] = {220.00, 261.63, 311.13, 440.00}; // A3, C4, Eb4, A4
  const int durations[] = {250, 250, 250, 500};
  for (int i = 0; i < 4; i++) {
    playTone(notes[i], durations[i]);
    delay(50);
  }
}




