#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Servo.h>
#include <ArduinoJson.h>

#define OBJECT_TRIG_PIN 8
#define OBJECT_ECHO_PIN 7
#define TRASH_TRIG_PIN 6
#define TRASH_ECHO_PIN 5
#define SERVO_PIN 3

#define BAUD_RATE 9600
#define LORA_FREQUENCY 433E6
#define SOUND_SPEED 0.0334 // cm/s
#define TRASH_CAN_HEIGHT 23 // cm
#define OPEN_DISTANCE 30 // cm

struct State {
	float trash_level;
	bool is_open;
};
bool operator==(const struct State &a, const struct State &b)
{
	return a.is_open == b.is_open && a.trash_level == b.trash_level;
}

struct State prev_state = {
	.trash_level = 0,
	.is_open = false,
};

Servo servo;

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

	if (!LoRa.begin(LORA_FREQUENCY)) {
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
	float distance = duration / 2 * SOUND_SPEED;

	return distance;
}

struct State get_state()
{
	float distance = get_distance(TRASH_TRIG_PIN, TRASH_ECHO_PIN);
	float trash_level = (1 - distance / TRASH_CAN_HEIGHT) * 100;
	trash_level = constrain(trash_level, 0, 100);

	bool is_open = get_distance(OBJECT_TRIG_PIN, OBJECT_ECHO_PIN) <= OPEN_DISTANCE;

	return {
		.trash_level = trash_level,
		.is_open = is_open,
	};
}

void sync_state(struct State &data)
{
	servo.write(data.is_open ? 180 : 0);
}

void lora_transmit(struct State &state)
{
	JsonDocument doc;

	doc["trash_level"] = state.trash_level;
	doc["is_open"] = state.is_open;

	LoRa.beginPacket();
	serializeJson(doc, LoRa);
	LoRa.endPacket(true);
}

void loop()
{
	struct State state = get_state();
	if (state == prev_state) {
		return;
	}

    prev_state = state;
	lora_transmit(state);
    sync_state(state);
}
