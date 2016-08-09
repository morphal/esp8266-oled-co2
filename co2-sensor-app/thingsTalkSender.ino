const int httpPort = 80;
void sentToTT() {
  HTTPClient http;

  String request = "http://api.thingspeak.com/update?api_key=0W9GGBE3671R7P4C&field1=" + String(lastAIQ);
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

