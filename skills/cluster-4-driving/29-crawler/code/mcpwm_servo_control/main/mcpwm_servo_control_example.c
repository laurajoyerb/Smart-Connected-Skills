#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_attr.h"

#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

//You can get these value from the datasheet of servo you use, in general pulse width varies between 1000 to 2000 mocrosecond
#define SERVO_MIN_PULSEWIDTH 650 //Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH 2150 //Maximum pulse width in microsecond
#define SERVO_MAX_DEGREE 100 //Maximum angle in degree upto which servo can rotate

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

/**
 * @brief Configure MCPWM module
 */
uint32_t speedpw = 0;
int status = 0;

void set_speed(){
    while(1){

      while(status){

        printf("\n---STOP to FULL BACKWARD---\n");
        for(speedpw = 1300; speedpw > 700; speedpw -= 50){
          vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        printf("\n---FULL BACKWARD to STOP---\n");
        for(speedpw = 700; speedpw < 1300; speedpw += 50){
          vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        printf("\n---STOP to FULL FORWARD---\n");
        for(speedpw = 1530; speedpw < 2100; speedpw += 50){
          vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        printf("\n---FULL FORWARD to STOP---\n");
        for(speedpw = 2100; speedpw > 1530; speedpw -= 50){
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
        for (count = 0; count < SERVO_MAX_DEGREE; count++) {
            printf("\nAngle of rotation: %d\n", count);
            angle = servo_per_degree_init(count);

            printf("Steering servo pulse width: %dus\n", angle);
            printf("ESC pulse width: %dus\n", speedpw);

            mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, speedpw);
            mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, angle);
            vTaskDelay(1000 / portTICK_PERIOD_MS);     //Add delay, since it takes time for servo to rotate, generally 100ms/60degree rotation at 5V
        }

        for (count = SERVO_MAX_DEGREE; count > 0; count--) {
            printf("\nAngle of rotation: %d\n", count);
            angle = servo_per_degree_init(count);

            printf("Steering servo pulse width: %dus\n", angle);
            printf("ESC pulse width: %dus\n", speedpw);

            mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, speedpw);
            mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, angle);
            vTaskDelay(1000 / portTICK_PERIOD_MS);     //Add delay, since it takes time for servo to rotate, generally 100ms/60degree rotation at 5V
        }
    }
}

void app_main(void)
{
    printf("Testing servo motor.......\n");
    xTaskCreate(mcpwm_servo_control, "mcpwm_servo_control", 4096, NULL, 5, NULL);
    xTaskCreate(set_speed, "set_speed", 1024, NULL, 5, NULL);
}
