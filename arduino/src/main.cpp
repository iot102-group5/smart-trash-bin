#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

#define TRIG_PIN 7
#define ECHO_PIN 6
#define TRASH_CAN_HEIGHT 40

#define LORA_SS 10
#define LORA_RST 9
#define LORA_DIO0 2

float get_distance()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  float duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration / 29 / 2;

  return distance;
}

float get_trash_level()
{
  float distance = get_distance();
  float trash_level = ((TRASH_CAN_HEIGHT - distance) / TRASH_CAN_HEIGHT) * 100;

  trash_level = constrain(trash_level, 0, 100);
  return trash_level;
}

void lora_transmit(float trash_level){
  LoRa.beginPacket();
  LoRa.print(trash_level);
  LoRa.endPacket();
}

void setup()
{
  Serial.begin(9600);
    
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
    
    if (!LoRa.begin(915E6)) {
        Serial.println("LoRa init failed!");
    }
    Serial.println("LoRa Initialized");
}

void loop()
{

}