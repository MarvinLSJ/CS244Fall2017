  #include "Arduino.h"
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <Wire.h>
  #include "MAX30105.h"
  #include "SparkFunLIS3DH.h"
  #include "Wire.h"
  #include "SPI.h"
  
  
  

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



// void sendRequest(int num){

//   uint32_t IR;
//   uint32_t RED;

//   IR=particleSensor.getIR();
//   RED=particleSensor.getRed();
//   Serial.println("Cliping the URL...");
//   String addURL = "?num="+ String(num) + "&IR=" + String(IR) + "&RED=" + String(RED);

//   http.addHeader("Content-Type","application/x-www-form-urlencoded");  
//   Serial.println("begin to transmit");
//   http.begin("http://169.234.22.162/work.php" + addURL);
//   // Serial.println(Json); 
//   int HttpCode = http.GET();
//   if(HttpCode == HTTP_CODE_OK){
//     Serial.println("num="+ String(num) + " IR=" + String(IR) + " RED=" + String(RED));
//     Serial.println("uploaded!");
//   }
//   else {
//     Serial.printf("fail: %s", http.errorToString(HttpCode).c_str());
//   }
//   http.end();
// }

MAX30105 particleSensor;
HTTPClient http;
LIS3DH myIMU; //Default constructor is I2C, addr 0x19.
// #define debug Serial //Uncomment this line if you're using an Uno or ESP

// void setup() {
//   // put your setup code here, to run once:  
//   Serial.begin(115200);
//   Serial.println("Initializing...");
//   wifiSetup();

//   // Serial.println("begin setup\n");
//   // Serial.begin(9600);
//   // delay(1000); //relax...
//   // Serial.println("Processor came out of reset.\n");
  
//   // myIMU.settings.adcEnabled = 1;
//   // //Note:  By also setting tempEnabled = 1, temperature data is available
//   // //on ADC3.  Temperature *differences* can be read at a rate of
//   // //1 degree C per unit of ADC3
//   // myIMU.settings.tempEnabled = 0;
//   // myIMU.settings.accelSampleRate = 50;  //Hz.  Can be: 0,1,10,25,50,100,200,400,1600,5000 Hz
//   // myIMU.settings.accelRange = 2;      //Max G force readable.  Can be: 2, 4, 8, 16
//   // myIMU.settings.xAccelEnabled = 0;
//   // myIMU.settings.yAccelEnabled = 0;
//   // myIMU.settings.zAccelEnabled = 0;
  
//   //Call .begin() to configure the IMU
//   myIMU.begin();

// }

String to_Json(float x, float y, float z){
  String s = "[" + String(x) + "," +String(y) + "," + String(z) + "]";
  return s;
}

String to_Json_sum(String data[], int len){
  String s = "data={\"position\":[";
  for(int i = 0; i < len - 1; i++) {
    s += data[i] + ",";
  }
  s += data[len - 1];
  s += "]}";
  return s;
}

void SendtoServer(String post_data){

  if (WiFi.status() != WL_CONNECTED){
    Serial.println("WIFI Error");
    return;
  }
  
  http.begin("http://169.234.4.40/acc.php/"); 
  Serial.print("Posting Data: ");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  Serial.println(post_data);
  int httpCode = http.POST(post_data);
  
  if(httpCode > 0) {
    Serial.printf("[HTTP] POST... code: %d\n", httpCode);
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void setup(){
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("setup Wifi...");
  wifiSetup();
  Serial.println("Accelerator Setup...");
  myIMU.settings.accelSampleRate = 50;
  myIMU.begin();
}

void loop(){
  float x,y,z;
  int batch_size = 150;
  String data[batch_size];

  for(int i=0; i<batch_size; i++){
    x = myIMU.readFloatAccelX();
    y = myIMU.readFloatAccelY();
    z = myIMU.readFloatAccelZ();
    data[i] = to_Json(x,y,z);
  }
  String post_data = to_Json_sum(data, batch_size);
  //Serial.println("post_data:" + post_data);
  SendtoServer(post_data);
  delay(1000);
}



