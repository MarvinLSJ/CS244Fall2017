  #include "Arduino.h"
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <Wire.h>
  #include "MAX30105.h"
  
  // // WiFi settings
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

MAX30105 particleSensor;
HTTPClient http;
#define debug Serial //Uncomment this line if you're using an Uno or ESP

void sendRequest(int num){

  uint32_t IR;
  uint32_t RED;

  IR=particleSensor.getIR();
  RED=particleSensor.getRed();
  Serial.println("Cliping the URL...");
  String addURL = "?num="+ String(num) + "&IR=" + String(IR) + "&RED=" + String(RED);

  http.addHeader("Content-Type","application/x-www-form-urlencoded");  
  Serial.println("begin to transmit");
  http.begin("http://169.234.22.162/work.php" + addURL);
  // Serial.println(Json); 
  int HttpCode = http.GET();
  if(HttpCode == HTTP_CODE_OK){
    Serial.println("num="+ String(num) + " IR=" + String(IR) + " RED=" + String(RED));
    Serial.println("uploaded!");
  }
  else {
    Serial.printf("fail: %s", http.errorToString(HttpCode).c_str());
  }
  http.end();
}

void setup()
{
  debug.begin(115200);
  debug.println("Initializing...");

  printMacAddress();
  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    debug.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  particleSensor.setup(); //Configure sensor. Use 6.4mA for LED drive
}

unsigned long startpoint;
int num = 0;
void loop()
{
  Serial.begin(115200);
  Serial.println("Initializing...");
  startpoint = millis();

  while(millis()-startpoint < 120000){
    
    if(millis()-startpoint==0) {
      num = 0;
      int powerLevel = 0x02;
      particleSensor.setSampleRate(50);
      particleSensor.setPulseAmplitudeRed(powerLevel);
      particleSensor.setPulseAmplitudeIR(powerLevel);
      particleSensor.setPulseAmplitudeGreen(powerLevel);
      particleSensor.setPulseAmplitudeProximity(powerLevel);
    }
    if(millis()-startpoint==120000) {
      num = 0;
      int powerLevel = 0x1F;
      particleSensor.setSampleRate(50);
      particleSensor.setPulseAmplitudeRed(powerLevel);
      particleSensor.setPulseAmplitudeIR(powerLevel);
      particleSensor.setPulseAmplitudeGreen(powerLevel);
      particleSensor.setPulseAmplitudeProximity(powerLevel);
    }
    if(millis()-startpoint==240000) {
      num = 0;
      int powerLevel = 0x7F;
      particleSensor.setSampleRate(50);
      particleSensor.setPulseAmplitudeRed(powerLevel);
      particleSensor.setPulseAmplitudeIR(powerLevel);
      particleSensor.setPulseAmplitudeGreen(powerLevel);
      particleSensor.setPulseAmplitudeProximity(powerLevel);
    }
    if(millis()-startpoint==360000) {
      num = 0;
      int powerLevel = 0xFF;
      particleSensor.setSampleRate(50);
      particleSensor.setPulseAmplitudeRed(powerLevel);
      particleSensor.setPulseAmplitudeIR(powerLevel);
      particleSensor.setPulseAmplitudeGreen(powerLevel);
      particleSensor.setPulseAmplitudeProximity(powerLevel);
    }
    sendRequest(num);  
    num++;
  }
}
