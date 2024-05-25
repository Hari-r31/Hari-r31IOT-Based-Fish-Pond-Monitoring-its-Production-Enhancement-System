#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL3aSZOSvJE"
#define BLYNK_TEMPLATE_NAME "IoT based fish pond monitoring system"
#define BLYNK_AUTH_TOKEN "oarjLtJNkigx7ZN3MISc8eQPSoTWnmlu"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
BlynkTimer timer;
const char* ssid = "YOUR_SSID";
const char* password = "PASSWORD";
void ConnectingBlynk() {
   Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
}
