/*
*  This sketch demonstrates how to scan WiFi networks.
*  The API is almost the same as with the WiFi Shield library,
*  the most obvious difference being the different file you need to include:
*/
#include "esp_wifi.h"
#include <Wire.h>
#include <EEPROM.h>
#include "SSD1306.h"

#define flipDisplay true
#define maxCh 13

int curChannel = 1;

SSD1306 display(0x3c, 5, 4);

void sniffer(uint8_t *buf, uint16_t len) {
	pkts++;
	if (buf[12] == 0xA0 || buf[12] == 0xC0) {
		deauths++;
	}
}

void setup()
{
	display.init();
	if (flipDisplay) display.flipScreenVertically();

	/* show start screen */
	display.clear();
	display.setFont(ArialMT_Plain_16);
	display.drawString(0, 0, "Packet-");
	display.drawString(0, 16, "Monitor");
	display.setFont(ArialMT_Plain_10);
	display.drawString(0, 40, "Copyright (c) 2017");
	display.drawString(0, 50, "Stefan Kremser");
	display.display();
	delay(2500);

	/* start Serial */
	Serial.begin(115200);

	/* load last saved channel */
	EEPROM.begin(4096);
	curChannel = EEPROM.read(2000);
	if (curChannel < 1 || curChannel > maxCh) {
		curChannel = 1;
		EEPROM.write(2000, curChannel);
		EEPROM.commit();
	}

	/* set pin modes for button and LED */
	//pinMode(btn, INPUT);
	//pinMode(ledPin, OUTPUT);

	/* setup wifi */
	wifi_set_opmode(STATION_MODE);
	wifi_promiscuous_enable(0);
	WiFi.disconnect();
	wifi_set_promiscuous_rx_cb(sniffer);
	wifi_set_channel(curChannel);
	wifi_promiscuous_enable(1);

	Serial.println("starting!");
	
}

void loop()
{
	
}
