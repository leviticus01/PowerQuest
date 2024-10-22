#include <driver/i2s.h>
#include <math.h>

// I2S configuration
#define I2S_NUM         I2S_NUM_0
#define I2S_BCLK        22
#define I2S_LRCLK       23
#define I2S_DOUT        21
#define SAMPLE_RATE     44100

// Note frequencies for the Pac-Man startup sound (in Hz)
const float NOTE_C4 = 261.63;
const float NOTE_E4 = 329.63;
const float NOTE_G4 = 392.00;
const float NOTE_C5 = 523.25;
const float NOTE_G3 = 196.00;
const float NOTE_C3 = 130.81;
const float NOTE_F4 = 349.23;
const float NOTE_A4 = 440.00;

// Duration for each note (in milliseconds)
const int durations[] = {150, 150, 150, 150, 300, 300, 150, 150, 150, 150, 300};

// Corresponding notes in sequence
const float notes[] = {
  NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, 
  NOTE_G3, NOTE_C3, NOTE_C4, NOTE_F4, 
  NOTE_A4, NOTE_C5, NOTE_G4
};

// Number of notes in the melody
const int numNotes = sizeof(notes) / sizeof(notes[0]);

void setup() {
  // I2S configuration
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S_MSB,
    .intr_alloc_flags = 0, // Default interrupt priority
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
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

void playTone(float frequency, int duration) {
  int16_t sample;
  size_t bytes_written;

  int numSamples = (SAMPLE_RATE * duration) / 1000;
  for (int i = 0; i < numSamples; i++) {
    sample = 1000 * sin(2 * PI * frequency * i / SAMPLE_RATE);
    i2s_write(I2S_NUM, &sample, sizeof(sample), &bytes_written, portMAX_DELAY);
  }
}

void loop() {
  // Play each note in the sequence with the corresponding duration
  for (int i = 0; i < numNotes; i++) {
    playTone(notes[i], durations[i]);
    delay(50); // Small delay between notes for better separation
  }

  delay(2000); // Wait before replaying the sequence
}
