//writen by Keith Matthews
//used examples from standard librarys as listed, and guides from Dronebot Workshop
//help from Josh Buker, (Mizu)

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>
#include <Wire.h>

#ifndef STASSID
#define STASSID "DESKTOP1FGUT8D"  //put your wifi SSID information here I recomend using a hotspot
#define STAPSK  "testtest" //put your wifi password here
#endif

#define LED 2

#define SLAVE_ADDR 9

int value = 0;

const int output1 = 14;

AsyncWebServer server(80);

const char* ssid = STASSID;
const char* password = STAPSK;

void recvMsg(uint8_t *data, size_t len){
  //WebSerial.println("Received Data...");
  String d = ""; 
  String headerd = "";
  String body = "";
  for(int i=0; i < len; i++){
    d += char(data[i]);
  }
  
  headerd = getValue(d, ',' ,0);
  
  //WebSerial.println(headerd);
  //WebSerial.println(d);
 if (headerd == "A"){
  String wval = getValue(d, ',', 1);
  String xval = getValue(d, ',', 2);
  String yval = getValue(d, ',', 3);
  String zval = getValue(d, ',', 4);
  
  if(isNumber(wval) &&  wval.toInt() == constrain(wval.toInt(), 0, 8000)){
    if(isNumber(xval) &&  xval.toInt() == constrain(xval.toInt(), 0, 8000)){
      if(isNumber(yval) &&  yval.toInt() == constrain(yval.toInt(), 0, 3)){
        if(isNumber(zval) &&  zval.toInt() == constrain(zval.toInt(), 0, 5)){
         if(yval.toInt() != zval.toInt()){
          body = (d.substring(2));
          WebSerial.println("Received Data...");
          WebSerial.println(headerd);
          WebSerial.println(body);
          Wire.beginTransmission(SLAVE_ADDR);
          Wire.write(body.c_str());
          Wire.endTransmission();
          d = ""; 
          headerd = "";
          body = "";
          }
         else if(yval.toInt() == 0 && zval.toInt() == 0){
          WebSerial.println("Error: both Direction value and Stop value are 0");
          }
         else if(yval.toInt() > 0 && zval.toInt() > 0){
          WebSerial.println("Error: both Direction value and Stop value are larger than 0");
          }
        }
        else{
          WebSerial.println("Error, 4th value must be an integer between 0 and 4, you entered: " + zval);
        }
       }
       else{
         WebSerial.println("Error, 3th value must be an integer between 0 and 2, you entered: " + yval);
       }
      }
     else{
     WebSerial.println("Error, 2th value must be an integer between 0 and 8000, you entered: " + xval);
     } 
     }
   else{
   WebSerial.println("Error, 1st value must be an integer between 0 and 8000, you entered: " + wval);
  }
 }
 
//  The below lines are functions to turn the LED on and off
// else if (d =="LED1+"){  //LED ON
//    analogWrite(output1, 0);
// }
// else if (d =="LED1-"){  //LED OFF
//    analogWrite(output1, 255);
// }

//Set LED brightness using values from 0-255, the closer to 255 the brigher the LED will be
 else if (headerd == "LED1"){    
    body = (d.substring(5));
    if(isNumber(body) && body.toInt() == constrain(body.toInt(), 0, 255)){
      WebSerial.println("Received Data...");
      WebSerial.println(headerd);
      WebSerial.println(body);
      analogWrite(output1, map(body.toInt(), 0, 255, 255, 0));
      d = ""; 
      headerd = "";
      body = "";
    }
    else{
      WebSerial.println("invalid Input Value for LED1 must be between 0 - 255, you entered: " + body);
    }
 }
 else if(d == "hA"){
  WebSerial.println("'A' = motor 1 syntax is: ");
  WebSerial.println("A,maxAcceleration,maxVelocity,directionOfMotor,stopValueOfMotor");
  WebSerial.println("maxAcceleration: int of 0 -> 8000");
  WebSerial.println("maxVelocity: int of 0 -> 8000");
  WebSerial.println("directionOfMotor: 0 stopped, 1 Clockwise, 2 counter clockwise");
  WebSerial.println("stopValueOfMotor: 0 Moving, 1 Hard-stop hold, 2 Hard-stop free spin -> ");
  WebSerial.println("stopValueOfMotor(cont.): 3 decelerate-stop hold, 4 decelerated-stop free spin");
  WebSerial.println("Ensure that there are always 4 values followed by A");
  //WebSerial.println("Either directionOfMotor or stopValueOfMotor must be zero. if both are non zero or both are zero you will get errors");
 }
else if(d == "hL"){
  WebSerial.println("'LED1' Header denotes backlight LED being called, the expected syntax is as follows: ");
  WebSerial.println("LED1,valueOfBrightness");
  WebSerial.println("the valueOfBrightness must be between 0 and 255, 0 denotes the most dim setting and 255 is the brightest");
  WebSerial.println("");
  
 }
 else{
  WebSerial.println("Error: Header value not understood.");
  WebSerial.println("Accepted Headers are 'A' , 'LED1'");
  WebSerial.println("enter 'hA' for syntax of 'A'");
  WebSerial.println("enter hL for syntax of 'LED1'"); 
  }
}

String getValue(String data, char separator, int index) //all hail get value for we know not what it does//
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

boolean isNumber(String input) {
  for(int i=0;i<input.length();i++) {
    if(!isDigit(input.charAt(i))) { return false; }
  }
  return true;
}


void setup() {
  // put your setup code here, to run once:
Wire.begin();

  analogWriteFreq(200);
  
  analogWrite(output1, 255);
  
  Serial.begin(9600);
  
  //pinMode(LED, OUTPUT);
  
  Serial.println("Booting");
  
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
 
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  // WebSerial is accessible at "<IP Address>/webserial" in browser
  WebSerial.begin(&server);
  WebSerial.msgCallback(recvMsg);
  server.begin();

  
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  // put your main code here, to run repeatedly:
  ArduinoOTA.handle();
  delay(100);
}
