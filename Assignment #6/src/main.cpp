  #include "Arduino.h"
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include "MAX30105.h"
  #include "SparkFunLIS3DH.h"
  #include "Wire.h"
  #include "SPI.h"
  #include <chrono>

  // WiFi settings
  const char *ssid = "UCInet Mobile Access";
  // const char *ssid = "CA33C1";
  // const char *password = "52ULAEG351721";
  void wifiSetup()
  {
      WiFi.begin(ssid);//, password);
      while (WiFi.status() != WL_CONNECTED)
      {
          delay(500);
          Serial.print(".");
      }
      Serial.println("");
      Serial.println("WiFi connected");
  }


MAX30105 particleSensor;
HTTPClient http;
LIS3DH myIMU; 


String to_Json(int16_t x, int16_t y, int16_t z,uint32_t IR,uint32_t RED){
  String s = "[" + String(x) + "," +String(y) + "," + String(z) + ','+ String(IR) + "," + String(RED) + "]";
  return s;
}

String to_Json_sum(String data[], int len){
  String s = "data={\"position\":[";
  for(int i = 0; i < len - 1; i++) {
    s += data[i] + ",";
  }
  s += data[len - 1];1
  s += "]}";
  return s;
}

unsigned long last_finish_time = millis();

void SendtoServer(String post_data){

  if (WiFi.status() != WL_CONNECTED){
    Serial.println("WIFI Error");
    return;
  }
  
  http.begin("http://169.234.7.227/acc.php/"); 
  Serial.print("Posting Data: ");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST(post_data);
  
  if(httpCode > 0) {
    Serial.printf("[HTTP] POST... code: %d\n", httpCode);
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      // Serial.println(payload);
      Serial.println("\nTime taken:\n" + String(millis() - last_finish_time));
      last_finish_time = millis();
    }
  } else {
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void setup(){
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println(millis());
  Serial.println("setup Wifi...");
  wifiSetup();
  Serial.println("Accelerator Setup...");
  myIMU.settings.accelSampleRate = 50;
  // myIMU.settings.accelRange = 16;
  Serial.println(myIMU.settings.accelRange);
  myIMU.begin();
  Serial.println("MAX30105 Setup...");
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  particleSensor.setup(); //Configure sensor. Use 6.4mA for LED drive
}

void loop(){
  float x,y,z;
  int batch_size = 50;
  String data[batch_size];

  uint32_t IR;
  uint32_t RED;

  for(int i=0; i<batch_size; i++){
    x = myIMU.readRawAccelX();
    y = myIMU.readRawAccelY();
    z = myIMU.readRawAccelZ();
    IR=particleSensor.getIR();
    RED=particleSensor.getRed();
    //data
    data[i] = to_Json(x,y,z,IR,RED);
  }
  String post_data = to_Json_sum(data, batch_size);
  Serial.println("post_data:" + post_data);
  SendtoServer(post_data);
  // Serial.println(post_data);
  // delay(1000);
}



