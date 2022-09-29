#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "freertos/event_groups.h"

const static char* TAG = "wifi";
 esp_netif_t * esp_netif;

static EventGroupHandle_t wifi_events;
static const int CONNECTED_GOT_IP = BIT0;
static const int DISCONNECTED = BIT1;

static void event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data){
    switch(event_id)
    {
        case SYSTEM_EVENT_STA_START:
            ESP_LOGI(TAG, "connecting");
            esp_wifi_connect();
            break;

        case SYSTEM_EVENT_STA_CONNECTED:
            ESP_LOGI(TAG, "connected");
            break;

        case SYSTEM_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "disconnected");
            xEventGroupSetBits(wifi_events, DISCONNECTED);
            break;

        case IP_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "got ip");
            xEventGroupSetBits(wifi_events, CONNECTED_GOT_IP);
            break;

    }
}



void wifi_init(void){
    esp_netif_init();
    esp_event_loop_create_default();
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_init_config);
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, event_handler, NULL);
    esp_wifi_set_storage(WIFI_STORAGE_RAM); //flash or ram

}

esp_err_t wifi_connect_sta(const char* ssid, const char* pass, int timeout){

    wifi_events = xEventGroupCreate();
    esp_netif = esp_netif_create_default_wifi_sta();

    wifi_config_t wifi_config;
    memset(&wifi_config,0,sizeof(wifi_config_t));

    strncpy((char *) wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid)-1);
    strncpy((char *) wifi_config.sta.password, pass, sizeof(wifi_config.sta.password)-1);

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();

    EventBits_t result = xEventGroupWaitBits(wifi_events, CONNECTED_GOT_IP | DISCONNECTED, pdTRUE, pdFALSE, pdMS_TO_TICKS(timeout));
    if (result == CONNECTED_GOT_IP)
        return 0;
    return -1;
}

void wifi_disconnect(void){
    esp_wifi_disconnect();
    esp_wifi_stop();
}