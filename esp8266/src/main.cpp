#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <LoRa.h>

const char *ssid = "Wifi_SSID";
const char *password = "Password";
const String id = "3fa85f64-5717-4562-b3fc-2c963f66afa6";
const char *server_url = ("https://trashcan-api.arisavinh.dev/device/" + id + "/data").c_str();

#define LORA_SS_PIN 15
#define LORA_RESET_PIN 16
#define LORA_DIO0_PIN 4

void send_data_to_server(String json_string)
{
  HTTPClient http;
  WiFiClient client;

  http.begin(client, server_url);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(json_string);

  if (httpResponseCode > 0)
  {
    String response = http.getString();
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    Serial.println("Response: " + response);
  }
  else
  {
    Serial.println("Error sending data: " + String(httpResponseCode));
  }

  http.end();
}

void setup()
{
  Serial.begin(115200);

  LoRa.setPins(LORA_SS_PIN, LORA_RESET_PIN, LORA_DIO0_PIN);

  if (!LoRa.begin(915E6))
  {
    Serial.println("LoRa initialization failed!");
  }
  Serial.println("LoRa initialization successful!");

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
}

const char* read_lora() {
  int packetSize = LoRa.parsePacket();

  if(packetSize == 0)
    return NULL;


  String received_data = "";
  while (LoRa.available())
  {
    received_data += (char)LoRa.read();
  }
  Serial.println("Received LoRa data: " + received_data);

  return received_data.c_str();
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    const char *received_data = read_lora();
    if(received_data != NULL)
      send_data_to_server(received_data);
  }
  else
  {
    Serial.println("WiFi connection lost");
    WiFi.begin(ssid, password);
  }

  delay(100);
}
