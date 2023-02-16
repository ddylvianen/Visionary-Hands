#include <driver/i2s.h>

byte vi_pin = 8;

class vibrator {
    // Connections to INMP441 I2S microphone
#define I2S_WS 12
#define I2S_SD 9
#define I2S_SCK 11
    // Use I2S Processor 0
#define I2S_PORT I2S_NUM_0

    // Define input buffer length
#define bufferLen 64
    int16_t sBuffer[bufferLen];
  public:
    vibrator() {
      setup();
    }
    //
    // Class Methods / functions
    void setup() {

    }


    void i2s_install() {
      // Set up I2S Processor configuration
      const i2s_config_t i2s_config = {
        .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = 44100,
        .bits_per_sample = i2s_bits_per_sample_t(16),
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = bufferLen,
        .use_apll = false
      };

      i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
    }

    void i2s_setpin() {
      // Set I2S pin configuration
      const i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK,
        .ws_io_num = I2S_WS,
        .data_out_num = -1,
        .data_in_num = I2S_SD
      };

      i2s_set_pin(I2S_PORT, &pin_config);
    }

    void set_up() {
      // Set up I2S
      i2s_install();
      i2s_setpin();
      i2s_start(I2S_PORT);

      delay(500);
    }

    float loop_sound() {
      // False print statements to "lock range" on serial plotter display
      // Change rangelimit value to adjust "sensitivity"
      int rangelimit = 3000;
      Serial.print(rangelimit * -1);
      Serial.print(" ");
      Serial.print(rangelimit);
      Serial.print(" ");

      // Get I2S data and place in data buffer
      size_t bytesIn = 0;
      esp_err_t result = i2s_read(I2S_PORT, &sBuffer, bufferLen, &bytesIn, portMAX_DELAY);

      if (result == ESP_OK)
      {
        // Read I2S data buffer
        int16_t samples_read = bytesIn / 8;
        if (samples_read > 0) {
          float mean = 0;
          for (int16_t i = 0; i < samples_read; ++i) {
            mean += (sBuffer[i]);
          }

          // Average the data reading
          mean /= samples_read;
          Serial.println(mean);


        }
      }
    }
};
