#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "driver/rmt.h"

static const char *RMT_TX_TAG = "RMT Tx";
static const int RX_BUF_SIZE = 1024;

#define TXD_PIN (25)
#define RXD_PIN (34)

#define RMT_TX_CHANNEL 1
#define RMT_TX_GPIO (26)

#define OFF 0
#define RED 1
#define GREEN 2
#define BLUE 3

#define RED_LED 15
#define GREEN_LED 32
#define BLUE_LED 14

#define BUTTON 4

int color = OFF;

void init(void) {
    const uart_config_t uart_config = {
        .baud_rate = 2400,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // We won't use a buffer for sending data.
    uart_set_line_inverse(UART_NUM_1, UART_INVERSE_RXD);
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
}

int sendData(const char* logName, const char* data)
{
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(UART_NUM_1, data, len);
    ESP_LOGI(logName, "Wrote %d bytes", txBytes);
    return txBytes;
}


//Convert uint8_t type of data to rmt format data.
static void IRAM_ATTR u8_to_rmt(const void* src, rmt_item32_t* dest, size_t src_size,
                         size_t wanted_num, size_t* translated_size, size_t* item_num)
{
    if(src == NULL || dest == NULL) {
        *translated_size = 0;
        *item_num = 0;
        return;
    }
    const rmt_item32_t bit0 = {{{ 32767, 1, 15000, 0 }}}; //Logical 0
    const rmt_item32_t bit1 = {{{ 32767, 1, 32767, 0 }}}; //Logical 1
    size_t size = 0;
    size_t num = 0;
    uint8_t *psrc = (uint8_t *)src;
    rmt_item32_t* pdest = dest;
    while (size < src_size && num < wanted_num) {
        for(int i = 0; i < 8; i++) {
            if(*psrc & (0x1 << i)) {
                pdest->val =  bit1.val;
            } else {
                pdest->val =  bit0.val;
            }
            num++;
            pdest++;
        }
        size++;
        psrc++;
    }
    *translated_size = size;
    *item_num = num;
}

/*
 * Initialize the RMT Tx channel
 */
static void rmt_tx_int(void)
{
    rmt_config_t config;
    config.rmt_mode = 0;
    config.channel = RMT_TX_CHANNEL;
    config.gpio_num = RMT_TX_GPIO;
    config.mem_block_num = 1;
    config.tx_config.loop_en = 0;
    config.tx_config.carrier_en = 1;
    config.tx_config.idle_output_en = 1;
    config.tx_config.idle_level = 1;
    config.tx_config.carrier_duty_percent = 50;
    config.tx_config.carrier_freq_hz = 38000;
    config.tx_config.carrier_level = 1;
    config.clk_div = 100;

    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));
    ESP_ERROR_CHECK(rmt_translator_init(config.channel, u8_to_rmt));
}

static void tx_task(void *arg)
{
    static const char *TX_TASK_TAG = "TX_TASK";
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
    while (1) {
      switch(color) {
        case RED:
          sendData(TX_TASK_TAG, "RED");
          break;
        case GREEN:
          sendData(TX_TASK_TAG, "GREEN");
          break;
        case BLUE:
          sendData(TX_TASK_TAG, "BLUE");
          break;
        case OFF:
          sendData(TX_TASK_TAG, "OFF");
          break;
      }
      vTaskDelay(150);
    }
}

static void rx_task(void *arg)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    while (1) {
        const int rxBytes = uart_read_bytes(UART_NUM_1, data, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
        if (rxBytes > 0) {
            data[rxBytes] = 0;
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
            if (strcmp((const char*) data, "RED") == 0) {
              gpio_set_level(RED_LED, 1);
              gpio_set_level(GREEN_LED, 0);
              gpio_set_level(BLUE_LED, 0);
            } else if (strcmp((const char*) data, "GREEN") == 0) {
              gpio_set_level(RED_LED, 0);
              gpio_set_level(GREEN_LED, 1);
              gpio_set_level(BLUE_LED, 0);
            } else if (strcmp((const char*) data, "BLUE") == 0) {
              gpio_set_level(RED_LED, 0);
              gpio_set_level(GREEN_LED, 0);
              gpio_set_level(BLUE_LED, 1);
            } else if (strcmp((const char*) data, "OFF") == 0) {
              gpio_set_level(RED_LED, 0);
              gpio_set_level(GREEN_LED, 0);
              gpio_set_level(BLUE_LED, 0);
            }
        }
    }
    free(data);
}

static void test_button_push() {
  int state;
  bool changed = false;
  while (1) {
    state = gpio_get_level(BUTTON);

    if (state == 0 && changed == false) {
      changed = true;
      color++;
      if (color > 3) {
        color = 0;
      }
    } else if (state == 1 && changed == true) {
      changed = false;
    }
    vTaskDelay(10);
  }
}

void app_main(void)
{
    init();

    ESP_LOGI(RMT_TX_TAG, "Configuring transmitter");
    rmt_tx_int();

    gpio_pad_select_gpio(RED_LED);
    gpio_set_direction(RED_LED, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(GREEN_LED);
    gpio_set_direction(GREEN_LED, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(BLUE_LED);
    gpio_set_direction(BLUE_LED, GPIO_MODE_OUTPUT);

    gpio_set_direction(BUTTON, GPIO_MODE_INPUT);

    xTaskCreate(rx_task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
    xTaskCreate(tx_task, "uart_tx_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);
    xTaskCreate(test_button_push, "test_button_push", 1024*2, NULL, configMAX_PRIORITIES -2, NULL);
}
