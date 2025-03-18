#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <SPI.h>
#include <LoRa.h>

const char *ssid = "yuu";
const char *password = "akagiyuu";
const char *server_url = "https://trashcan-api.arisavinh.dev/device/3fa85f64-5717-4562-b3fc-2c963f66afa6/data";

#define LORA_SS_PIN 15
#define LORA_RESET_PIN 16
#define LORA_DIO0_PIN 4
#define LORA_FREQUENCY 433E1

volatile bool lora_data_received = false;
String lora_received_data = "";

void send_data_to_server()
{
    BearSSL::WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    http.begin(client, server_url);
    http.addHeader("Content-Type", "application/json");

    // Serial.println("Sending POST request with JSON: " + json_string);

    Serial.print("data: ");
    Serial.println(lora_received_data);
    int httpResponseCode = http.POST(lora_received_data);
    Serial.println("HTTP Response code: " + String(httpResponseCode));

    if (httpResponseCode > 0)
    {
        String response = http.getString();
        Serial.println("Response: " + response);
    }
    else
    {
        Serial.println("Error sending data: " + String(httpResponseCode));
    }

    http.end();
}

void on_receive(int packet_size)
{
    if (packet_size == 0)
        return;

    lora_received_data = "";
    while (LoRa.available())
    {
        lora_received_data += (char)LoRa.read();
    }
    lora_data_received = true;
}

void setup()
{
    Serial.begin(9600);

    LoRa.setPins(LORA_SS_PIN, LORA_RESET_PIN, LORA_DIO0_PIN);
    if (!LoRa.begin(LORA_FREQUENCY))
    {
        Serial.println("LoRa initialization failed!");
    }
    LoRa.onReceive(on_receive);
    LoRa.receive();

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
}

void loop()
{
    if (lora_data_received)
    {
        lora_data_received = false;
        if (WiFi.status() == WL_CONNECTED)
        {
            send_data_to_server();
        }
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi connection lost");
        WiFi.begin(ssid, password);
    }

    delay(1000);
}