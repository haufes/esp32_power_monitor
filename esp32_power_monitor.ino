
extern "C" {
#include "soc/pcnt_struct.h"
}
#include "driver/pcnt.h"
#include <ArduinoJson.h>

const int led_1_pin = 2;
const int led_1_blink_ms = 20;

const int meter_pin = 5;
#define PCNT_METER_UNIT PCNT_UNIT_0
#define PCNT_H_LIM_VAL 32767
#define PCNT_L_LIM_VAL -1

const int output_every = 60000;

void setup() {
  pinMode(meter_pin,INPUT);
  pinMode(led_1_pin, OUTPUT);
  Serial.begin(115200);
  init_counter();
}

void loop() {
  int meter_count = readPcntCounter_0();  
  digitalWrite(led_1_pin, HIGH);
  StaticJsonDocument<100> jsonBuffer;

  jsonBuffer["meter_count"] = meter_count;
  serializeJson(jsonBuffer, Serial);
  Serial.println();
  delay(led_1_blink_ms);
  digitalWrite(led_1_pin, LOW);
  
  delay(output_every);
}


//initialize counter for power meter
void init_counter() {
  
  pcnt_config_t pcnt_config = {
    meter_pin, // Pulse input gpio_num, if you want to use gpio16, pulse_gpio_num = 16, a negative value will be ignored
    PCNT_PIN_NOT_USED, // Control signal input gpio_num, a negative value will be ignored
    PCNT_MODE_KEEP, // PCNT low control mode
    PCNT_MODE_KEEP, // PCNT high control mode
    PCNT_COUNT_INC, // PCNT positive edge count mode
    PCNT_COUNT_DIS, // PCNT negative edge count mode
    PCNT_H_LIM_VAL, // Maximum counter value
    PCNT_L_LIM_VAL, // Minimum counter value
    PCNT_METER_UNIT, // PCNT unit number
    PCNT_CHANNEL_0, // the PCNT channel
  };
  
  pcnt_unit_config(&pcnt_config);//init unit    
  pcnt_filter_enable(PCNT_METER_UNIT);
  pcnt_set_filter_value(PCNT_METER_UNIT,500);
  pcnt_intr_disable(PCNT_METER_UNIT);
  pcnt_event_disable(PCNT_METER_UNIT, PCNT_EVT_L_LIM);
  pcnt_event_disable(PCNT_METER_UNIT, PCNT_EVT_H_LIM);
  pcnt_event_disable(PCNT_METER_UNIT, PCNT_EVT_THRES_0);
  pcnt_event_disable(PCNT_METER_UNIT, PCNT_EVT_THRES_1);
  pcnt_event_disable(PCNT_METER_UNIT, PCNT_EVT_ZERO);
  pcnt_counter_pause(PCNT_METER_UNIT);
  pcnt_counter_clear(PCNT_METER_UNIT);
  pcnt_intr_enable(PCNT_METER_UNIT);  
  pcnt_counter_resume(PCNT_METER_UNIT);

}

int readPcntCounter_0() {
  int16_t value;
  if(pcnt_get_counter_value(PCNT_METER_UNIT, &value) == ESP_OK){
      pcnt_counter_clear(PCNT_METER_UNIT);
      return (int) value;
  }
  

}
