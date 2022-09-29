#include <stdio.h>
#include "connect.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "aht20.h"

#define aht20_addr 0x38
const static char* TAG = "wifi";

static i2c_config_t aht20_i2c_conf = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = 16, //default pin for SDA
    .scl_io_num = 17, //default pin for SCL
    .sda_pullup_en = GPIO_PULLUP_DISABLE,
    .scl_pullup_en = GPIO_PULLUP_DISABLE,
    .master.clk_speed = 100000
};

void aht20_init(gpio_num_t clock, gpio_num_t data) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    aht20_i2c_conf.sda_io_num = data;
    aht20_i2c_conf.scl_io_num = clock;
}

static void i2c_setup(){

    i2c_param_config(I2C_NUM_0, &aht20_i2c_conf);
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
}


esp_err_t on_client_data(esp_http_client_event_t *evt){
    switch(evt->event_id)
    {
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG,"Length=%d",evt->data_len);
            printf("%.*s\n",evt->data_len, (char *)evt->data);
            break;

        default:
            break;
    }
    return ESP_OK;
}

void rest(){
    while(1){
        esp_http_client_config_t config = {
            .host = "10.0.0.80",
            .port = 3001,
            .path = "/weather",
            .event_handler = on_client_data,
            .method = HTTP_METHOD_PUT
        };
        char buffer[100];
        sprintf(buffer, "{\"temp\":\"%3.2f\", \"humidity\":\"%3.2f\"}",aht20_data.temperature, aht20_data.rel_humidity);
        //const char *post_data = "{\"temp\":\"100c\", \"humidity\":\"300percent\"}";
        const char *post_data = buffer;
        esp_http_client_handle_t client = esp_http_client_init(&config);
        esp_http_client_set_header(client, "Content-Type","application/json");
        esp_http_client_set_post_field(client, post_data, strlen(post_data)); //PLACEHOLDER
        esp_err_t err = esp_http_client_perform(client);

        if (err== ESP_OK){
            ESP_LOGI(TAG, "HTTP STATUS = %d", (int)esp_http_client_get_status_code(client));
        }
        else{
            ESP_LOGI(TAG, "SHITS FUCKED MAN: %s", esp_err_to_name(err));
        }
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}



void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    aht20_init(17,16);
    i2c_setup();

    wifi_init();
    ESP_ERROR_CHECK(wifi_connect_sta("Smith", "7867023537", 10000));
    xTaskCreatePinnedToCore(&rest, "api", 10096,NULL,0,NULL,1 );
    xTaskCreatePinnedToCore(&aht20_read_measures, "task_read_ath20",  10096, NULL, 0, NULL, 1);
}
