
void init_wifi()
{  
  //reset settings - for testing
  //wifiManager.resetSettings();
  
  wifiManager.setDebugOutput(true);
 
  //устанавливаем колбэк, который будет вызван при входе в конфиг режим
  wifiManager.setAPCallback(configModeCallback);

  if(wifiManager.tryConnect())
  {    
    connectedToWifi();
  }  
}

void handleAPmode() {
  
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
                connectedToWifi();
              }
          }           
        }
}

void connectedToWifi()
{
    Serial.printf("ENTERING CONNECTED TO WIFI HANDLER\n");
  
    currentSSID = WiFi.SSID();
    currentIp = ipToString(WiFi.localIP());

    //ticker.attach(4, tick);
    
    //HTTP_init();
    
    //запускаем SSDP сервис 
    Serial.printf("Starting SSDP...\n");
    SSDP_init();
    Serial.printf("SSDP Ready!\n");

    ui.transitionToFrame(1);
    
    Serial.printf("Start CO2 measuring\n");
    measureTicker.attach(10, co2Mesaure);
}

void configModeCallback (WiFiManager *myWiFiManager) {

  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());

  String ssid = myWiFiManager->getConfigPortalSSID();
    
  currentSSID = ssid;
  currentIp = ipToString(WiFi.localIP()); 
  
  Serial.println(ssid);
}

void HTTP_init(void){
  
  HTTP.on("/index.html", HTTP_GET, [](){
      HTTP.send(200, "text/plain", "Test aREST");
    });
    HTTP.on("/description.xml", HTTP_GET, [](){
      SSDP.schema(HTTP.client());
    });
    HTTP.begin();
}

void SSDP_init(void){
  
    SSDP.setSchemaURL("description.xml");
    SSDP.setHTTPPort(80);
    SSDP.setName("WiFiManager + SSDP");
    SSDP.setSerialNumber("001788102201");
    SSDP.setURL("index.html");
    SSDP.setModelName("WiFiManager + SSDP");
    SSDP.setModelNumber("000000000001");
    SSDP.setModelURL("http://esp8266-arduinoide.ru/wifimanager/");
    SSDP.setManufacturer("Tretyakov Sergey");
    SSDP.setManufacturerURL("http://www.esp8266-arduinoide.ru");
    SSDP.begin();
}

String ipToString(IPAddress ip){
  String s="";
  for (int i=0; i<4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  return s;
}
