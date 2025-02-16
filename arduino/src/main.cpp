#include <Arduino.h>

#define TRIG_PIN 7
#define ECHO_PIN 6
#define TRASH_CAN_HEIGHT 40

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

float get_fill_level_percentage()
{
  float distance = get_distance();
  float fill_level_percentage = ((TRASH_CAN_HEIGHT - distance) / TRASH_CAN_HEIGHT) * 100;

  fill_level_percentage = constrain(fill_level_percentage, 0, 100);
  return fill_level_percentage;
}

void setup()
{
}

void loop()
{
}