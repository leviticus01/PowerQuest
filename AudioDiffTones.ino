#include <driver/i2s.h>
#include <math.h>

// I2S configuration
#define I2S_NUM         I2S_NUM_0
#define I2S_BCLK        22
#define I2S_LRCLK       23
#define I2S_DOUT        21
#define SAMPLE_RATE     44100

void setup() {
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
}

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

// Variation 1: Sine wave melody
void playMelody1() {
  const float notes[] = {261.63, 329.63, 392.00, 523.25}; // C4, E4, G4, C5
  const int durations[] = {200, 200, 200, 400};
  for (int i = 0; i < 4; i++) {
    playTone(notes[i], durations[i]);
    delay(50);
  }
}

// Variation 2: Ascending and Descending Arpeggio
void playMelody2() {
  const float notes[] = {261.63, 329.63, 392.00, 523.25, 392.00, 329.63, 261.63};
  const int durations[] = {150, 150, 150, 300, 150, 150, 300};
  for (int i = 0; i < 7; i++) {
    playTone(notes[i], durations[i]);
    delay(50);
  }
}

// Variation 3: Random Frequencies for a Futuristic Sound
void playMelody3() {
  for (int i = 0; i < 8; i++) {
    float randomFreq = random(200, 800); // Random frequency between 200Hz and 800Hz
    playTone(randomFreq, 200);
    delay(30);
  }
}

// Variation 4: Minor Chord Melody
void playMelody4() {
  const float notes[] = {220.00, 261.63, 311.13, 440.00}; // A3, C4, Eb4, A4
  const int durations[] = {250, 250, 250, 500};
  for (int i = 0; i < 4; i++) {
    playTone(notes[i], durations[i]);
    delay(50);
  }
}

void loop() {
  playMelody1();
  delay(1000);
  playMelody2();
  delay(1000);
  playMelody3();
  delay(1000);
  playMelody4();
  delay(1000);
}
