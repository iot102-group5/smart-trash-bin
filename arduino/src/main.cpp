#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Servo.h>
#include <ArduinoJson.h>

#define BAUD_RATE 9600
#define LORA_FREQUENCY 433E6

#define OBJECT_TRIG_PIN 8
#define OBJECT_ECHO_PIN 7
#define TRASH_TRIG_PIN 6
#define TRASH_ECHO_PIN 5
#define SERVO_PIN 3

#define TRASH_CAN_HEIGHT 40
#define OPEN_DISTANCE 10

Servo servo;
float trash_level;
bool is_open;

void onTxDone()
{
    Serial.println("Send data success");
}

void setup()
{
    Serial.begin(BAUD_RATE);

    pinMode(OBJECT_TRIG_PIN, OUTPUT);
    pinMode(OBJECT_ECHO_PIN, INPUT);

    pinMode(TRASH_TRIG_PIN, OUTPUT);
    pinMode(TRASH_ECHO_PIN, INPUT);

    servo.attach(SERVO_PIN, 600, 2300);

    if (!LoRa.begin(LORA_FREQUENCY))
    {
        Serial.println("LoRa init failed!");
    }
    LoRa.onTxDone(onTxDone);
}

float get_distance(int trig_pin, int echo_pin)
{
    digitalWrite(trig_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(trig_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig_pin, LOW);

    float duration = pulseIn(echo_pin, HIGH);
    float distance = duration / 29 / 2;

    return distance;
}

float get_trash_level()
{
    float distance = get_distance(TRASH_TRIG_PIN, TRASH_ECHO_PIN);
    float trash_level = (1 - distance / TRASH_CAN_HEIGHT) * 100;
    trash_level = constrain(trash_level, 0, 100);
    return trash_level;
}

bool get_is_open()
{
    return get_distance(OBJECT_TRIG_PIN, OBJECT_ECHO_PIN) <= OPEN_DISTANCE;
}

void set_trash_can(bool is_open)
{
    servo.write(is_open ? 90 : 0);
}

void lora_transmit(float trash_level, bool is_open)
{
    JsonDocument doc;

    doc["is_open"] = is_open;
    doc["trash_level"] = trash_level;

    LoRa.beginPacket();
    serializeJson(doc, LoRa);
    LoRa.endPacket(true);
}

void loop()
{
    trash_level = get_trash_level();
    Serial.println(trash_level);
    is_open = get_is_open();
    Serial.println(is_open);
    set_trash_can(is_open);
    lora_transmit(trash_level, is_open);
    delay(5000);
}