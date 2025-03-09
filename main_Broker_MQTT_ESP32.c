#include <stdio.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "mqtt_client.h"
#include "esp_event.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_adc_cal.h"
#include "driver/adc.h"

static const char *MQTT_TAG = "MQTT_Client";
static const char *WIFI_TAG = "WiFi";
static const char *ESP_TAG = "ESP_System";

uint8_t sensor_state;
uint16_t potenciometer_state;
 char *topic = "v1/devices/me/telemetry";
 char payload[50];

QueueHandle_t sensor_queue;
QueueHandle_t potenciometer_queue;
esp_mqtt_client_handle_t mqtt_client;

#define WIFI_SSID "LIB-3321567"
#define WIFI_PASS "UpbkFd6qfr9e"

#define THINGSBOARD_URL "mqtt://thingsboard.cloud:1883"
#define THINGSBOARD_TOKEN "jpkwvr6W6XAzwpCnyOQ1"

#define HIGH 1
#define LOW 0

esp_err_t mqtt_client_launch(void);



long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void mqtt_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
esp_mqtt_event_handle_t event = event_data;
switch (event->event_id)
{

case MQTT_EVENT_DELETED:
ESP_LOGI(MQTT_TAG, "{Evento : DELETED}: La Publicacion del Mensaje [%s] ha Sido Eliminada", event->topic);
break;

case MQTT_EVENT_PUBLISHED:
ESP_LOGI(MQTT_TAG, "{Evento : PUBLISHED}: La Publicacion del Mensaje [%s] ha Sido Exitosa", event->topic);
break;

case MQTT_EVENT_ERROR:
ESP_LOGW(MQTT_TAG, "{Evento : ERROR}: El Broker [%s] a Notificado de un Error Desconocido... ", THINGSBOARD_URL);
break;

case MQTT_EVENT_BEFORE_CONNECT:
ESP_LOGI(MQTT_TAG, "{Evento : BEFORE_CONNECT}: Estableciendo Conexion Con el Broker [%s]", THINGSBOARD_URL);
break;

case MQTT_EVENT_DISCONNECTED:
    ESP_LOGE(MQTT_TAG, "Conexion Con el Broker [%s] Perdida, Retomando la Conexion", THINGSBOARD_URL);
    for(uint8_t i = 0; esp_mqtt_client_reconnect(mqtt_client) == ESP_FAIL; i++){
 vTaskDelay(1000 / portTICK_PERIOD_MS);
 if(i >= 10){
 ESP_LOGE(MQTT_TAG, "{Evento : DISCONNECT}: Reintentos de Reconexion Excedidos, Reiniciando Configuraciones Del MQTT Client...");
 ESP_ERROR_CHECK(esp_mqtt_client_stop(mqtt_client));
 if(esp_mqtt_client_destroy(mqtt_client)){
     ESP_LOGE(MQTT_TAG, "Reinicio Del MQTT Client Fallido, Reiniciando Sistema...");
     esp_restart();
 };
 mqtt_client_launch();
 }
    }
    ESP_LOGI(MQTT_TAG, "Conexion Con El Broker [%s] Restablecida", THINGSBOARD_URL);
    break;

case MQTT_EVENT_CONNECTED:
    ESP_LOGI(MQTT_TAG,"{Evento : CONNECT}: La Conexion Con el Broker [%s] es Estable y Funcional", THINGSBOARD_URL);
    break;

case MQTT_EVENT_DATA:
ESP_LOGI(MQTT_TAG, "{Evento : DATA}: Mensaje Recibido:[\n Topic: %.*s\n Data: %.*s]", event->topic_len, event->topic, event->data_len, event->data);
    break;
    
default:
ESP_LOGW(MQTT_TAG, "[Warning]: Evento No Reconocido");
    break;
}

}

esp_err_t mqtt_client_launch(void){
    
esp_mqtt_client_config_t mqtt_cfg = {
    .broker.address.uri = THINGSBOARD_URL,
    .credentials.username = THINGSBOARD_TOKEN
};

mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
if(mqtt_client == NULL){
ESP_LOGE(MQTT_TAG,"{Error}: No Se Pudo Cargar La Configuracion Del MQTT Client desde: [esp_mqtt_client_init(&mqtt_cfg)]");
return ESP_FAIL;
}
ESP_LOGI(MQTT_TAG, "Configuraciones De MQTT Inicializadas...");

ESP_ERROR_CHECK(esp_mqtt_client_register_event(mqtt_client, MQTT_EVENT_ANY, mqtt_event_handler, NULL));
ESP_LOGI(MQTT_TAG, "Manejador de Eventos de MQTT Client Inicializado...");

if(esp_mqtt_client_start(mqtt_client) == ESP_FAIL){
ESP_LOGE(MQTT_TAG, "{Error}: No Se Pudo Iniciar El MQTT Client"); 
return ESP_FAIL;
}
ESP_LOGI(MQTT_TAG, "Cliente MQTT Inicializado Con Exito...");

return ESP_OK;
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){

if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START){
ESP_LOGI(WIFI_TAG, "EL Modo WiFi STA ha Sido Inicializado Con Exito");
ESP_LOGI(WIFI_TAG, "Conectando...");
for(uint8_t i = 0; esp_wifi_connect() != ESP_OK; i++){
vTaskDelay(1000 / portTICK_PERIOD_MS);
ESP_LOGI(WIFI_TAG, "Conectando...");
if(i >= 10){
ESP_LOGE(WIFI_TAG, "Reintentos de Reconexion Excedidos, Reiniciando el Sistema...");
esp_restart();
}
} 
}

if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED){
ESP_LOGI(WIFI_TAG, "Estacion WiFi (STA) Conectada a la Red %s", WIFI_SSID);
ESP_LOGI(WIFI_TAG, "Iniciando Obtencion De Datos IP, Mask, Gateway...");
}

if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED){
ESP_LOGW(WIFI_TAG, "Conexion WiFi Interrumpida, Reconectando...");
for(uint8_t i = 0; esp_wifi_connect() == ESP_FAIL; i++){
vTaskDelay(100 / portTICK_PERIOD_MS);
if(i >= 10){
ESP_LOGE(WIFI_TAG, "Reintentos de Reconexion Excedidos, Reiniciando el Sistema...");
esp_restart();    
}
}
ESP_LOGI(WIFI_TAG, "Reconexion Exitosa, Retomando la Comunicacion...");
}
if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP){
ip_event_got_ip_t *ip_data = (ip_event_got_ip_t*) event_data;
ESP_LOGI(WIFI_TAG, "Datos de Red Obtenidos Con Exito: IP:" IPSTR "Mask:" IPSTR "Gateway:" IPSTR, 
    IP2STR(&ip_data->ip_info.ip),
    IP2STR(&ip_data->ip_info.netmask),
    IP2STR(&ip_data->ip_info.gw)
    );


}
}
esp_err_t wifi_default_connect(void){
ESP_LOGI(WIFI_TAG, "Iniciando Las Configuraciones WiFi Necesarias...");
ESP_ERROR_CHECK(esp_netif_init());
ESP_ERROR_CHECK(esp_event_loop_create_default());

esp_netif_t *wifi_netif = esp_netif_create_default_wifi_sta();
assert(wifi_netif);
ESP_LOGI(WIFI_TAG, "Interface WiFi Inicializada");
wifi_init_config_t wifi_cfg = WIFI_INIT_CONFIG_DEFAULT();

esp_wifi_init(&wifi_cfg);

esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL);
esp_event_handler_register(IP_EVENT, IP_EVENT_GOT_IP6, &wifi_event_handler, NULL);
ESP_LOGI(WIFI_TAG,"Manejador De Eventos WiFi Iniciado");
wifi_config_t wifi_config = {
    .sta = {
        .ssid = WIFI_SSID,
        .password = WIFI_PASS,
        .threshold.rssi = -120
    },

};
ESP_LOGI(WIFI_TAG, "Credenciales WiFi Configuradas...");

esp_wifi_set_mode(WIFI_MODE_STA);
esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
esp_wifi_start();
ESP_LOGI(WIFI_TAG, "Protocolo WiFi Inicializado Con Exito, Procediendo a la Conexion Con la Red: %s", WIFI_SSID);

return ESP_OK;
}

void sensor_task(void* pvParameters){
gpio_set_direction(GPIO_NUM_4, GPIO_MODE_INPUT);
gpio_set_pull_mode(GPIO_NUM_4, GPIO_PULLUP_ONLY);

for(;;){
potenciometer_state = adc1_get_raw(ADC1_CHANNEL_6);    
sensor_state = !gpio_get_level(GPIO_NUM_4);

    xQueueSend(sensor_queue, &sensor_state, 1000 / portTICK_PERIOD_MS);
    ESP_LOGI(ESP_TAG,"Sensor Real Value: %d", sensor_state);
    xQueueSend(potenciometer_queue, &potenciometer_state, 1000 / portTICK_PERIOD_MS);
    ESP_LOGI(ESP_TAG,"Potenciometer Real Value: %d", potenciometer_state);

vTaskDelay(2000 / portTICK_PERIOD_MS);
 }
}

void mqtt_send_task(void* pvParameters){

int16_t Humidity = 0;
int16_t Temperature = 0;
int16_t dB = 0;

for(;;){



if(xQueueReceive(sensor_queue, &sensor_state, 1000 / portTICK_PERIOD_MS) == pdPASS){
    sprintf(payload, "{\"sensor\": %d}", sensor_state);
    ESP_LOGI(ESP_TAG,"Sensor Value: %d", sensor_state);
    esp_mqtt_client_publish(mqtt_client, topic, payload, strlen(payload), 0, 0);
}
else{ESP_LOGW(ESP_TAG,"Recibimiento fallido de sensor_state a la cola sensor_queue");}

if(xQueueReceive(potenciometer_queue, &potenciometer_state, 1000 / portTICK_PERIOD_MS) == pdPASS){
    Humidity = map(potenciometer_state, 0, 4095, 0, 100);
    Temperature = map(potenciometer_state, 0, 4095, -30, 150);
    dB = map(potenciometer_state, 0, 4095, 0, 300);
    ESP_LOGI(ESP_TAG,"Potenciometer Value: %d", potenciometer_state);
    sprintf(payload, "{\"potenciometer\": %d}", potenciometer_state);
    esp_mqtt_client_publish(mqtt_client, topic, payload, strlen(payload), 0, 0);
    sprintf(payload, "{\"humidity\": %d}", Humidity);
    esp_mqtt_client_publish(mqtt_client, topic, payload, strlen(payload), 0, 0);
    sprintf(payload, "{\"temperature\": %d}", Temperature);
    esp_mqtt_client_publish(mqtt_client, topic, payload, strlen(payload), 0, 0);
    sprintf(payload, "{\"dB\": %d}", dB);
    esp_mqtt_client_publish(mqtt_client, topic, payload, strlen(payload), 0, 0);
}
else{ESP_LOGW(ESP_TAG,"Recibimiento fallido de potenciometer_state a la cola potenciometer_queue");}
vTaskDelay(2000 / portTICK_PERIOD_MS);

 }
}







void app_main(void)
{
esp_reset_reason_t reason_reset = esp_reset_reason();
    /* Configure the peripheral according to the LED type */
switch (reason_reset)
{
case ESP_RST_PANIC:
    ESP_LOGW(ESP_TAG, "Reinicio Por Panico (Intentos o Errores Excesivos)");
    break;

case ESP_RST_SW:
    ESP_LOGW(ESP_TAG, "Reinicio Por Software");
    break;

case ESP_RST_PWR_GLITCH:
ESP_LOGW(ESP_TAG, "Reinicio Por Falla De Alimentacion (Caida de Voltaje)");
    break;

default:
ESP_LOGI(ESP_TAG, "Causa De Reinicio Desconocida");
    break;
}

adc1_channel_t adc = ADC1_CHANNEL_2;
adc_atten_t adc_config = ADC_ATTEN_DB_12;
adc_bits_width_t adc_width = ADC_WIDTH_BIT_12;

adc1_config_channel_atten(adc, adc_config);


ESP_LOGI(ESP_TAG, "Intentando Iniciar Memoria No Volatil (nvs)");
esp_err_t req = nvs_flash_init();
if (req == ESP_ERR_NVS_NOT_INITIALIZED || req == ESP_ERR_NVS_NO_FREE_PAGES) {
ESP_LOGI(ESP_TAG, "Intento Fallido, Reacondicionando Memoria No Volatil (nvs)");    
ESP_ERROR_CHECK(nvs_flash_erase());
ESP_ERROR_CHECK(nvs_flash_init());
ESP_LOGI(ESP_TAG, "Memoria No Volatil (nvs) Iniciada Correctamente");
}
else{ESP_LOGI(ESP_TAG, "Memoria No Volatil (nvs) Iniciada Correctamente");}


ESP_LOGI(WIFI_TAG, "Inicializando WiFi...");
for(uint8_t i = 0; wifi_default_connect() != ESP_OK; i++){
ESP_LOGI(WIFI_TAG, "Intento Fallido, Reintentando Conexion Con WiFi...");    
vTaskDelay(100 / portTICK_PERIOD_MS);
esp_wifi_deinit();
if(i >= 10){
ESP_LOGE(WIFI_TAG, "Reintentos de Inicializacion Excedidos, Reiniciando el Sistema...");
esp_restart();    
}
}
vTaskDelay(10000 / portTICK_PERIOD_MS);

for(uint8_t i = 0; mqtt_client_launch() != ESP_OK; i++){
vTaskDelay(100 / portTICK_PERIOD_MS);
esp_wifi_deinit();
if(i >= 5){
ESP_LOGE(WIFI_TAG, "Reintentos de Inicializacion Excedidos, Reiniciando el Sistema...");
esp_restart();    
}
}

sensor_queue = xQueueCreate(5, sizeof(uint8_t));
potenciometer_queue = xQueueCreate(5, sizeof(uint32_t));

xTaskCreate(sensor_task, "sensor_task", 4*1024, NULL, 5, NULL);
xTaskCreate(mqtt_send_task, "mqtt_task", 8*1024, NULL, 5, NULL);

}
