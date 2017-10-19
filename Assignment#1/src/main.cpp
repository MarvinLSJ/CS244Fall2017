  #include "Arduino.h"
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  
  String deviceName = "CS244";
  
  // WiFi settings
  const char *ssid = "UCInet Mobile Access";

  void printMacAddress()
  {
      byte mac[6];
      WiFi.macAddress(mac);
  
      char MAC_char[18]="";
      for (int i = 0; i < sizeof(mac); ++i)
      {
          sprintf(MAC_char, "%s%02x:", MAC_char, mac[i]);
      }
  
      Serial.print("Mac address : ");
      Serial.print(MAC_char);
  
      WiFi.begin(ssid);//, password);
      while (WiFi.status() != WL_CONNECTED)
      {
          delay(500);
          Serial.print(".");
      }
      Serial.println("");
      Serial.println("WiFi connected");
      // Print the IP address
      Serial.println(WiFi.localIP());
  }
  
  void setup()
  {
    // initialize LED digital pin as an output.
    pinMode(LED_BUILTIN, OUTPUT);
      
    Serial.begin(115200);
    Serial.println("Program started");
    printMacAddress();
  }
  
  void postData() {
     //the path and file to send the data to:

     Serial.println("Start to connect...");
    if (WiFi.status() != WL_CONNECTED){
      Serial.println("WIFI Error");
      return;
    }
    HTTPClient http;
    http.addHeader("Content-Type","application/x-www-form-urlencoded");  
    //Transmit data - GET  
    http.begin("http://169.234.14.162//cs244.php?name=hello_from_ESP!");
    int HttpCode = http.GET();
    if(HttpCode == HTTP_CODE_OK){
      String payload = http.getString();
      Serial.println(payload);
    }
    else {
      Serial.printf("fail: %s", http.errorToString(HttpCode).c_str());
    }
    http.end();

  }
  
  void loop()
  {
    // printMacAddress();
    // You can write your logic here
    // Below are the LED samples from the hands-on
  
    // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_BUILTIN, HIGH);
    // wait for a second
    delay(1000);
    // turn the LED off by making the voltage LOW
    digitalWrite(LED_BUILTIN, LOW);
     // wait for a second
    delay(1000);
    postData();
  }

  