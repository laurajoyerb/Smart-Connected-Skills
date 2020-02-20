#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include <string.h>
#include "driver/uart.h"
#include "esp_vfs_dev.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include <stdlib.h>

#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_HS_CH0_GPIO       (13)
#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0

#define LEDC_LS_TIMER          LEDC_TIMER_1
#define LEDC_LS_MODE           LEDC_LOW_SPEED_MODE

#define LEDC_TEST_DUTY         (4000)
#define LEDC_TEST_FADE_TIME    (3000)

void app_main(void)
{
    /* Install UART driver for interrupt-driven reads and writes */
    ESP_ERROR_CHECK( uart_driver_install(UART_NUM_0,
      256, 0, 0, NULL, 0) );

    /* Tell VFS to use UART driver */
    esp_vfs_dev_uart_use_driver(UART_NUM_0);

    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_13_BIT, // resolution of PWM duty
        .freq_hz = 5000,                      // frequency of PWM signal
        .speed_mode = LEDC_HS_MODE,           // timer mode
        .timer_num = LEDC_HS_TIMER,            // timer index
        .clk_cfg = LEDC_AUTO_CLK,              // Auto select the source clock
    };
    // Set configuration of timer0 for high speed channels
    ledc_timer_config(&ledc_timer);

    // Prepare and set configuration of timer1 for low speed channels
    ledc_timer.speed_mode = LEDC_LS_MODE;
    ledc_timer.timer_num = LEDC_LS_TIMER;
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
            .channel    = LEDC_HS_CH0_CHANNEL,
            .duty       = 0,
            .gpio_num   = LEDC_HS_CH0_GPIO,
            .speed_mode = LEDC_HS_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_HS_TIMER
    };

    ledc_channel_config(&ledc_channel);

    // Initialize fade service.
    ledc_fade_func_install(0);

    int duty = 5000;

    char buf[10];

    while (1) {
        printf("Led Intensity (0-9): ");
        gets(buf);
        printf("%s\n", buf);

        if (strcmp(buf, "cycle") == 0) {
          duty = 0;
          int intensity = 0;
          while (duty < 5000) {
            printf("Intensity %d\n", intensity);
            ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, duty);
            ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
            vTaskDelay(25);
            duty += 500;
            intensity++;
          }

          vTaskDelay(25);
          duty = 4000;
          intensity = 8;
          while (duty >= 0) {
            printf("Intensity %d\n", intensity);
            ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, duty);
            ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
            vTaskDelay(25);
            duty -= 500;
            intensity--;
          }
        } else {
          int num = atoi(buf);

          switch(num) {
            case 0:
              duty = 0;
              break;
            case 1:
              duty = 500;
              break;
            case 2:
              duty = 1000;
              break;
            case 3:
              duty = 1500;
              break;
            case 4:
              duty = 2000;
              break;
            case 5:
              duty = 2500;
              break;
            case 6:
              duty = 3000;
              break;
            case 7:
              duty = 3500;
              break;
            case 8:
              duty = 4000;
              break;
            case 9:
              duty = 4500;
              break;
            default:
              printf("Enter a value between 0 and 9 inclusive.\n");
              break;
          }

          printf("LEDC set duty = %d\n", duty);
          ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, duty);
          ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
        }
    }
}
