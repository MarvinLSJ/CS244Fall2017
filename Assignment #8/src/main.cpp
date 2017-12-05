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
// WiFiClient client;


String to_Json(float x, float y, float z,uint32_t IR,uint32_t RED){
  String s = "[" + String(x) + "," +String(y) + "," + String(z) + ','+ String(IR) + "," + String(RED) + "]";
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

unsigned long last_finish_time = millis();

void SendtoServer(String post_data){

  if (WiFi.status() != WL_CONNECTED){
    Serial.println("WIFI Error");
    return;
  }
  
  http.begin("http://169.234.54.179/acc.php/"); 
  Serial.print("Posting Data: ");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  // http.addHeader("Content-Length", String(post_data.length()));
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

// void SendbyClient(String post_data){
// Serial.begin(115200);
// while(!Serial){}
// WiFiClient client;
// const char* host="http://169.234.54.179/acc.php/";

// if (client.connect(host, 80)) {
//  client.println("POST /posts HTTP/1.1");
//  client.println("Host: jsonplaceholder.typicode.com");
//  client.println("Cache-Control: no-cache");
//  client.println("Content-Type: application/x-www-form-urlencoded");
//  client.print("Content-Length: ");
//  client.println(post_data.length());
//  client.println();
//  client.println(post_data);

//  long interval = 75;
//  unsigned long currentMillis = millis(), previousMillis = millis();
//  while(!client.available()){

//    if( (currentMillis - previousMillis) > interval ){

//      Serial.println("Timeout");
//     //  blinkLed.detach();
//      digitalWrite(2, LOW);
//      client.stop();     
//      return;
//    }
//    currentMillis = millis();
//  }

//  while (client.connected())
//  {
//    if ( client.available() )
//    {
//      char str=client.read();
//     Serial.println(str);
//    }      
//  }
// }
// }

void setup(){
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println(millis());
  Serial.println("setup Wifi...");
  wifiSetup();
  Serial.println("Accelerator Setup...");
  myIMU.settings.accelSampleRate = 50;
  // myIMU.settings.accelRange = 16;
  // Serial.println(myIMU.settings.accelRange);
  myIMU.begin();
  Serial.println("MAX30105 Setup...");
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  particleSensor.setup(); //Configure sensor. Use 6.4mA for LED drive
  // Serial.println("00000" );
}

void loop(){
  float x,y,z;
  int batch_size = 75;
  // Serial.println("0" );
  String data[batch_size];
  // Serial.println("1" );
  uint32_t IR;
  uint32_t RED;

  for(int i=0; i<batch_size; i++){
    
    x = myIMU.readFloatAccelX();
    y = myIMU.readFloatAccelY();
    z = myIMU.readFloatAccelZ();
    IR=particleSensor.getIR();
    RED=particleSensor.getRed();
    // Serial.println("2"+ String(x));
    //data
    // delay(20);


    data[i] = to_Json(x,y,z,IR,RED);
    // Serial.println("3"+ String(data[i]));
  }

  String post_data = to_Json_sum(data, batch_size);
  Serial.println("post_data:"+ post_data);

  SendtoServer(post_data);
  // SendbyClient(post_data);



  // Serial.println(post_data);
  // delay(1000);
}



