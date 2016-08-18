const int httpPort = 8080;

//const char* host = "10.168.11.224";
const char* host = "api.thingspeak.com";
const char* thingspeak_key = "0W9GGBE3671R7P4C";

void sendToTT() {
  HTTPClient http;

  //String request = "http://api.thingspeak.com/update?api_key=0W9GGBE3671R7P4C&field1=" + String(lastAIQ);
  String request = "api.thingspeak.com/update?api_key=0W9GGBE3671R7P4C&field1=" + String(lastAIQ);
  Serial.println(request);
  http.begin(request); //HTTP

  int httpCode = http.GET();
  
  // httpCode will be negative on error
  if(httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if(httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          Serial.println(payload);
      }
  } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

void sendToTT2() {

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  String ppm = String(lastAIQ);
  String url = "/update?key=";
  url += thingspeak_key;
  url += "&field1=";
  url += ppm;
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection. going to sleep...");
}
