#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "string.h"

#define ECHO_TEST_TXD  (GPIO_NUM_17)
#define ECHO_TEST_RXD  (GPIO_NUM_16)
#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)

#define BUF_SIZE (1024)

static void echo_task(void *arg)
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);
    uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);

    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(UART_NUM_1, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        // printf("reading something: %d bytes\n", len);
        unsigned char words[12];

        for (int i = 0; i < 12; i++) {
          words[i] = (unsigned char) data[i];
          //printf("Byte %d: %x \n", i, words[i]);
        }

      //  printf("%d", words[0]);
        for (int i = 0; i < 9; i++) {
          if (words[i] == 89 && words[i+1] == 89)   // if the start byte is correct
          {
            char str[10];
            sprintf(str, "%x%x", words[i+3], words[i+2]);
            int cm = (int)strtol(str, NULL, 16);
            float m = (float) cm / 100;
            printf("Distance: %.2fm\n", m);
            break;
          }
        }
        vTaskDelay(30);
    }
}

void app_main(void)
{
    xTaskCreate(echo_task, "uart_echo_task", 4*1024, NULL, 10, NULL);
}
