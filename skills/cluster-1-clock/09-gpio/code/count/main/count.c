#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define RED_LED 12
#define GREEN_LED 27
#define BLUE_LED 33
#define YELLOW_LED 15

void app_main(void)
{
    gpio_pad_select_gpio(RED_LED);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(RED_LED, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(GREEN_LED);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(GREEN_LED, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(BLUE_LED);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLUE_LED, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(YELLOW_LED);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(YELLOW_LED, GPIO_MODE_OUTPUT);

    while(1) {
      // 0
      gpio_set_level(YELLOW_LED, 0);
      gpio_set_level(BLUE_LED, 0);
      gpio_set_level(GREEN_LED, 0);
      gpio_set_level(RED_LED, 0);
      vTaskDelay(1000 / portTICK_PERIOD_MS);

      // 1
      gpio_set_level(YELLOW_LED, 1);
      gpio_set_level(BLUE_LED, 0);
      gpio_set_level(GREEN_LED, 0);
      gpio_set_level(RED_LED, 0);
      vTaskDelay(1000 / portTICK_PERIOD_MS);

      // 2
      gpio_set_level(YELLOW_LED, 0);
      gpio_set_level(BLUE_LED, 1);
      gpio_set_level(GREEN_LED, 0);
      gpio_set_level(RED_LED, 0);
      vTaskDelay(1000 / portTICK_PERIOD_MS);

      // 3
      gpio_set_level(YELLOW_LED, 1);
      gpio_set_level(BLUE_LED, 1);
      gpio_set_level(GREEN_LED, 0);
      gpio_set_level(RED_LED, 0);
      vTaskDelay(1000 / portTICK_PERIOD_MS);

      // 4
      gpio_set_level(YELLOW_LED, 0);
      gpio_set_level(BLUE_LED, 0);
      gpio_set_level(GREEN_LED, 1);
      gpio_set_level(RED_LED, 0);
      vTaskDelay(1000 / portTICK_PERIOD_MS);

      // 5
      gpio_set_level(YELLOW_LED, 1);
      gpio_set_level(BLUE_LED, 0);
      gpio_set_level(GREEN_LED, 1);
      gpio_set_level(RED_LED, 0);
      vTaskDelay(1000 / portTICK_PERIOD_MS);

      // 6
      gpio_set_level(YELLOW_LED, 0);
      gpio_set_level(BLUE_LED, 1);
      gpio_set_level(GREEN_LED, 1);
      gpio_set_level(RED_LED, 0);
      vTaskDelay(1000 / portTICK_PERIOD_MS);

      // 7
      gpio_set_level(YELLOW_LED, 1);
      gpio_set_level(BLUE_LED, 1);
      gpio_set_level(GREEN_LED, 1);
      gpio_set_level(RED_LED, 0);
      vTaskDelay(1000 / portTICK_PERIOD_MS);

      // 8
      gpio_set_level(YELLOW_LED, 0);
      gpio_set_level(BLUE_LED, 0);
      gpio_set_level(GREEN_LED, 0);
      gpio_set_level(RED_LED, 1);
      vTaskDelay(1000 / portTICK_PERIOD_MS);

      // 9
      gpio_set_level(YELLOW_LED, 1);
      gpio_set_level(BLUE_LED, 0);
      gpio_set_level(GREEN_LED, 0);
      gpio_set_level(RED_LED, 1);
      vTaskDelay(1000 / portTICK_PERIOD_MS);

      // 10
      gpio_set_level(YELLOW_LED, 0);
      gpio_set_level(BLUE_LED, 1);
      gpio_set_level(GREEN_LED, 0);
      gpio_set_level(RED_LED, 1);
      vTaskDelay(1000 / portTICK_PERIOD_MS);

      // 11
      gpio_set_level(YELLOW_LED, 1);
      gpio_set_level(BLUE_LED, 1);
      gpio_set_level(GREEN_LED, 0);
      gpio_set_level(RED_LED, 1);
      vTaskDelay(1000 / portTICK_PERIOD_MS);

      // 12
      gpio_set_level(YELLOW_LED, 0);
      gpio_set_level(BLUE_LED, 0);
      gpio_set_level(GREEN_LED, 1);
      gpio_set_level(RED_LED, 1);
      vTaskDelay(1000 / portTICK_PERIOD_MS);

      // 13
      gpio_set_level(YELLOW_LED, 1);
      gpio_set_level(BLUE_LED, 0);
      gpio_set_level(GREEN_LED, 1);
      gpio_set_level(RED_LED, 1);
      vTaskDelay(1000 / portTICK_PERIOD_MS);

      // 14
      gpio_set_level(YELLOW_LED, 0);
      gpio_set_level(BLUE_LED, 1);
      gpio_set_level(GREEN_LED, 1);
      gpio_set_level(RED_LED, 1);
      vTaskDelay(1000 / portTICK_PERIOD_MS);

      // 15
      gpio_set_level(YELLOW_LED, 1);
      gpio_set_level(BLUE_LED, 1);
      gpio_set_level(GREEN_LED, 1);
      gpio_set_level(RED_LED, 1);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
