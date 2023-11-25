#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include "wifi_helper.h"
#include "secrets.h"

WifiHelper wh;

const IPAddress STATIC_IP(192, 168, 238, 230);
const IPAddress GATEWAY(192, 168, 238, 1);
const IPAddress SUBNET(255, 255, 255, 0);
const IPAddress DNS(GATEWAY);

const char DEVICE_NAME[] = "Meme-Bell Sensor";

void setup() {

  wh.setup();

  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.println();

  //delay(1500); // just for debugging
  Serial.println("\nStarting.");
  uint32_t ts_start = millis();
  bool res = wh.connect(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Timing connect(): "); Serial.print(millis()-ts_start); Serial.println("ms");
  Serial.print("Result="); Serial.println(res);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Still not connected");
    delay(50);
  }

  WiFiClient client;
  HTTPClient http;

  Serial.print("[HTTP] begin...\n");
  http.begin(client, "https://discord.com/api/v10/users/@me/channels");

  http.addHeader("Authorization", DISCORD_API_TOKEN);
  http.addHeader("User-Agent", "DiscordBot (https://github.com/KubikDezimeter/meme-bell_sensor, 0.1.0)");

  const char* user_ids[] = {"137225481446621184"};
  const int user_count = 1;

  for (int i = 0; i < user_count; i++) {
    char body[1024];
    snprintf(body, sizeof(body), "{\"recipient_id\":\"%s\"}", user_ids[i]);
    // Serial.println(body);
    // Serial.println(sizeof(user_ids));
    int httpCode = http.POST(body);

    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);
      const String& payload = http.getString();
      Serial.println("received payload:\n<<");
      Serial.println(payload);
      Serial.println(">>");

    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
  }

  http.end();
  ESP.deepSleep(0);
}

void loop() {
  yield();
}