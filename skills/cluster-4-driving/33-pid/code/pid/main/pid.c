#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling

#define RED (33)
#define BLUE (27)
#define GREEN (12)

#define HIGH 0
#define LOW 1

static esp_adc_cal_characteristics_t *adc_chars;
static const adc_channel_t channel = ADC_CHANNEL_6;     //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_1;

int dt = 100;
float integral;
float derivative;

int setpoint = 50;
int range;

// Flag for dt
int dt_complete = 0;

static void PID() {

  int Kp = 1;
  int Ki = 1;
  int Kd = 1;
  float previous_error = 0.00;
  integral = 0.00;

  while(1)
  {
    float error = setpoint - range;
    if (error > 0) {
      gpio_set_level(RED, LOW);
      gpio_set_level(GREEN, LOW);
      gpio_set_level(BLUE, HIGH);
    } else if (error < 0) {
      gpio_set_level(RED, HIGH);
      gpio_set_level(GREEN, LOW);
      gpio_set_level(BLUE, LOW);
    } else if (error == 0) {
      gpio_set_level(RED, LOW);
      gpio_set_level(GREEN, HIGH);
      gpio_set_level(BLUE, LOW);
    }
    integral = integral + error * dt;
    derivative = (error - previous_error) / dt;
    float output = Kp * error + Ki * integral + Kd * derivative;
    previous_error = error;
    printf("PID found the following: \n\tError:\t\t%.0f \n\tIntegral:\t%.1f \n\tDerivative:\t%.1f \n\tOutput is: \t%.1f\n\n", error, integral, derivative, output);
    vTaskDelay(dt);
  }
}

static int getRange(int voltage) {
  int distance = 0;

  if (voltage > 1500)
      distance = 0;
  else if (voltage <= 1500 && voltage > 1200)
      distance = 20;
  else if (voltage <= 1200 && voltage > 1000)
      distance = 30;
  else if (voltage <= 1000 && voltage > 700)
      distance = 40;
  else if (voltage <= 700 && voltage > 600)
      distance = 50;
  else if (voltage <= 600 && voltage > 500)
      distance = 60;
  else if (voltage <= 500 && voltage > 450)
      distance = 70;
  else if (voltage <= 450 && voltage > 400)
      distance = 80;
  else if (voltage <= 400 && voltage > 375)
      distance = 90;
  else if (voltage <= 375 && voltage > 350)
      distance = 100;
  else if (voltage <= 350 && voltage > 325)
      distance = 110;
  else if (voltage <= 325 && voltage > 300)
      distance = 120;
  else if (voltage <= 300 && voltage > 275)
      distance = 130;
  else if (voltage <= 275 && voltage > 250)
      distance = 140;
  else if (voltage < 250)
      distance = 150;

  return distance;
}

static void read_ir() {
  //Continuously sample ADC1
  while (1) {
      uint32_t adc_reading = 0;
      //Multisampling
      for (int i = 0; i < NO_OF_SAMPLES; i++) {
          if (unit == ADC_UNIT_1) {
              adc_reading += adc1_get_raw((adc1_channel_t)channel);
          } else {
              int raw;
              adc2_get_raw((adc2_channel_t)channel, ADC_WIDTH_BIT_12, &raw);
              adc_reading += raw;
          }
      }
      adc_reading /= NO_OF_SAMPLES;
      //Convert adc_reading to voltage in mV
      uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
      range = getRange(voltage);
      printf("Distance: %dcm\n", range);
      vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void app_main(void)
{
    //Configure ADC
    if (unit == ADC_UNIT_1) {
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(channel, atten);
    } else {
        adc2_config_channel_atten((adc2_channel_t)channel, atten);
    }

    //Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);

    gpio_pad_select_gpio(RED);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(RED, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(GREEN);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(GREEN, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(BLUE);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLUE, GPIO_MODE_OUTPUT);

    gpio_set_level(RED, LOW);
    gpio_set_level(GREEN, LOW);
    gpio_set_level(BLUE, LOW);

    xTaskCreate(PID,"PID", 1024*2, NULL, configMAX_PRIORITIES, NULL);
    xTaskCreate(read_ir,"read_ir", 1024*2, NULL, configMAX_PRIORITIES, NULL);
}
