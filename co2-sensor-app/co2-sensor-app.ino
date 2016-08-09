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

void tick()
{
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  
  init_oled();
  
  init_wifi();  
}

void loop() {
  // put your main code here, to run repeatedly:
  int remainingTimeBudget = ui.update();
  
  //Serial.println(remainingTimeBudget);
  if (remainingTimeBudget > 0) {
    /// если не подключены к внешней сети wifi 
    if(!wifiManager.isConectedAsClient())
    {   
        // проверяем включенность портала настройки
        if(!wifiManager.isConfigPortalStarted())
        {
          //стартуем конфигурационнный портал
          wifiManager.startConfigPortal("esp-wifi");  
        }
        else
        {
          // если включен портал, то пытаемся обработать введенные в нем данные
          if(wifiManager.processConfigPortalEnteredData() == true)
          {
              //если портал сказал, что введенные данные можно проверить на подключаемость -- пробуем
              if(wifiManager.resetAndCheckConnection())
              {                
                currentSSID = WiFi.SSID();
                currentIp = ipToString(WiFi.localIP());  
              }
          }           
        }
    }
    else
    {
      // You can do some work here
      // Don't do stuff if you are below your
      // time budget.
      //delay(remainingTimeBudget);
      HTTP.handleClient();
      delay(1);
    }    
  }
}
