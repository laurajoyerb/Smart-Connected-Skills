#include <stdio.h>
#include <string.h>
#include "driver/uart.h"
#include "esp_vfs_dev.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include <stdlib.h>

#define LED 13

void app_main()
{
    /* Install UART driver for interrupt-driven reads and writes */
    ESP_ERROR_CHECK( uart_driver_install(UART_NUM_0,
      256, 0, 0, NULL, 0) );

    /* Tell VFS to use UART driver */
    esp_vfs_dev_uart_use_driver(UART_NUM_0);

    gpio_pad_select_gpio(LED);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    printf("toggle mode\n");
    int state = 0; // 0: toggle mode; 1: echo mode; 2: hex to dec mode
    bool led = false; // false for off, true for on

    char buf[10];

    while(1) {
      switch(state) {
        case 0: // toggle led mode
          printf("Read: ");
          gets(buf);
          printf("%s\n", buf);
          if (buf[0] == 't') {
            led = !led;
            gpio_set_level(LED, led);
            break;
          }
          if (buf[0] == 's') {
            state = 1;
            printf("echo mode\n");
            break;
          }
          break;
        case 1: // echo mode
          printf("echo: ");
          gets(buf);
          printf("%s\n", buf);
          if (buf[0] == 's' && buf[1] == '\0') {
            state = 2;
            printf("dec to hex mode\n");
            break;
          }
          break;
        case 2: // dec to hex mode
          // algorithm from https://www.geeksforgeeks.org/program-decimal-hexadecimal-conversion/
          printf("Enter an integer: ");
          gets(buf);
          printf("%s\n", buf);
          if (buf[0] == 's') {
            state = 0;
            printf("toggle mode\n");
            break;
          }
          printf("Hex: ");
          char hex[30];
          int index = 0;
          int num = atoi(buf);
          while (num != 0) {
            int rem = num % 16;
            if (rem >= 10) {
              hex[index] = rem + 55;
            } else {
              hex[index] = rem + 48;
            }
            index++;
            num /= 16;
          }
          for (int i = index - 1; i >= 0; i--) {
            printf("%c", hex[i]);
          }
          printf("\n");
          break;
      }

      vTaskDelay(50 / portTICK_RATE_MS);
    }
}
