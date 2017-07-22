#include <Arduino.h>

#include "WiFi.h"
#include <SSD1306.h>
//#include "EEPROM.h"


SSD1306 display(0x3c, 5, 4);

void setup()
{
    Serial.begin(115200);
    display.init();
    display.flipScreenVertically();
    display.clear();
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.println("Setup done");
}

void loop()
{
    Serial.println("scan start");

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");

            display.setTextAlignment(TEXT_ALIGN_LEFT);
            display.setFont(ArialMT_Plain_10);
            display.drawString(0, (i*10), WiFi.SSID(i));
            display.display();

            delay(10);

        }
    }
    Serial.println("");

    // Wait a bit before scanning again
    delay(5000);
    display.clear();

}
