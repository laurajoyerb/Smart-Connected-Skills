#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "esp_attr.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

// For ADC
#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling

// For wheels
#define SERVO_MIN_PULSEWIDTH 650 //Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH 2150 //Maximum pulse width in microsecond
#define SERVO_MAX_DEGREE 100 //Maximum angle in degree upto which servo can rotate

// For ADC
static esp_adc_cal_characteristics_t *adc_chars;
static const adc_channel_t channel = ADC_CHANNEL_6;     //GPIO34 if ADC1 (A2)
static const adc_atten_t atten = ADC_ATTEN_DB_0;
static const adc_unit_t unit = ADC_UNIT_1;

bool pulsed = false;

int count;

// For ADC
static void check_efuse(void)
{
    //Check TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("eFuse Two Point: NOT supported\n");
    }

    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        printf("eFuse Vref: Supported\n");
    } else {
        printf("eFuse Vref: NOT supported\n");
    }
}

static void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("Characterized using eFuse Vref\n");
    } else {
        printf("Characterized using Default Vref\n");
    }
}

void adc_task(void *arg)
{
  //Check if Two Point or Vref are burned into eFuse
  check_efuse();

  // Initialize counter
  count = 0;

  //Configure ADC
  if (unit == ADC_UNIT_1) {
      adc1_config_width(ADC_WIDTH_BIT_12);
      adc1_config_channel_atten(channel, atten);
  } else {
      adc2_config_channel_atten((adc2_channel_t)channel, atten);
  }

  //Characterize ADC
  adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
  print_char_val_type(val_type);

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

      if (adc_reading < 4095 && pulsed == false)
      {
        count++;
        pulsed = true;
      } else if (adc_reading == 4095) {
        pulsed = false;
      }

      // printf("Raw: %d\n", adc_reading);
      // printf("Count: %d\n", count);
      vTaskDelay(pdMS_TO_TICKS(10));
  }
}

static void timer() {
  while(1) {
    count = 0;
    vTaskDelay(100);
    printf("Count: %d\n", count);
    float speed = (count * (2*3.14159*7/6))/100; //0.005; //(count / 6) * 0.62;
    printf("Speed is %.1fm/s\n", speed);
    count = 0;
  }
}

// For wheels
static void mcpwm_example_gpio_initialize(void)
{
    printf("initializing mcpwm servo control gpio......\n");
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, 27);    //Set GPIO 27 as PWM0A, to which servo is connected
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, 12);    //Set GPIO 12 as PWM0B
}

static uint32_t servo_per_degree_init(uint32_t degree_of_rotation)
{
    uint32_t cal_pulsewidth = 0;
    cal_pulsewidth = (SERVO_MIN_PULSEWIDTH + (((SERVO_MAX_PULSEWIDTH - SERVO_MIN_PULSEWIDTH) * (degree_of_rotation)) / (SERVO_MAX_DEGREE)));
    return cal_pulsewidth;
}

void calibrateESC() {
  printf("Turn on crawler in 3 sec: \n");
  vTaskDelay(3000 / portTICK_PERIOD_MS);  // Give yourself time to turn on crawler

  printf("Set to HIGH\n");
  mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 2100); // HIGH signal in microseconds
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  printf("Set to LOW\n");
  mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 700);  // LOW signal in microseconds
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  printf("Set to NEUTRAL\n");
  mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 1400); // NEUTRAL signal in microseconds
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  printf("Set to NEUTRAL! Calibration DONE\n");
  mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 1400); // reset the ESC to neutral (non-moving) value
}

uint32_t speedpw = 0;
int status = 0;

void set_speed(){
    while(1){

      while(status){

        for(speedpw = 1286; speedpw < 1289; speedpw += 1){
          vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        for(speedpw = 1289; speedpw > 1286; speedpw -= 1){
          vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

      }
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void mcpwm_servo_control(void *arg)
{
    uint32_t angle, count;
    //1. mcpwm gpio initialization
    mcpwm_example_gpio_initialize();

    //2. initial mcpwm configuration
    printf("Configuring Initial Parameters of mcpwm......\n");
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 50;    //frequency = 50Hz, i.e. for every servo motor time period should be 20ms
    pwm_config.cmpr_a = 0;    //duty cycle of PWMxA = 0
    pwm_config.cmpr_b = 0;    //duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    //Configure PWM0A & PWM0B with above settings

    printf("Calibrating ESC......\n");
    calibrateESC();

    status = 1;

    while (1) {
      mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, speedpw);
      vTaskDelay(1000 / portTICK_PERIOD_MS);     //Add delay, since it takes time for servo to rotate, generally 100ms/60degree rotation at 5V
    }
}

void app_main(void)
{
  printf("Doing my best\n");
  xTaskCreate(adc_task, "adc_task", 4096, NULL, 5, NULL);
  xTaskCreate(timer, "timer", 4096, NULL, 5, NULL);
  xTaskCreate(mcpwm_servo_control, "mcpwm_servo_control", 4096, NULL, 5, NULL);
  xTaskCreate(set_speed, "set_speed", 4096, NULL, 5, NULL);
}
