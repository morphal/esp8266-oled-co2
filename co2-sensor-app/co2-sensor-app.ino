/* Начало: для oled*/
#include <Wire.h>
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
// Include the UI lib
#include "OLEDDisplayUi.h"
// Include custom images
#include "images.h"

#define OLED_WIDTH 128
/* Конец: для oled*/

/*Начало: для wifi*/
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include "WiFiManager.h"         //https://github.com/tzapu/WiFiManager
#include <ESP8266SSDP.h>

/*Конец: для wifi*/

/// работает на 4 и 5 порту
#define SDA_PIN 4
#define SCL_PIN 5
#define LED_PIN 2

// Initialize the OLED display using Wire library
SSD1306  display(0x3c, SDA_PIN, SCL_PIN);
OLEDDisplayUi ui     ( &display );

// Web интерфейс для устройства
ESP8266WebServer HTTP(80);
WiFiManager wifiManager;

String currentSSID = "";
String currentIp = "";

//for LED status
#include <Ticker.h>
Ticker ticker;
Ticker measureTicker;

float valAIQ = 0.0;
float lastAIQ = 0.0;

void tick()
{
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  if (wifiManager.isConectedAsClient())
  {
    //ui.nextFrame();
  }
}

bool needToMeasureCo2 = false;

void co2Mesaure()
{
  needToMeasureCo2 = true;
  
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);

  init_oled();
  ui.update();

  //ticker.attach(0.5, tick);

  init_wifi();

  co2_init();
}

void loop() {
  // put your main code here, to run repeatedly:
  int remainingTimeBudget = ui.update();

  //Serial.println(remainingTimeBudget);
  if (remainingTimeBudget > 0) {
    /// если не подключены к внешней сети wifi
    if (!wifiManager.isConectedAsClient())
    {
      handleAPmode();
    }
    else
    {
      // You can do some work here
      // Don't do stuff if you are below your
      // time budget.
      //delay(remainingTimeBudget);
      HTTP.handleClient();
      delay(1);

      if(needToMeasureCo2)
      {
        measureTicker.detach();
        co2_measure();
        sendToTT2();
        needToMeasureCo2 = false;
        measureTicker.attach(10, co2Mesaure);
      }
    }
  }
}
