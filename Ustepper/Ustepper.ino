#include <uStepperSLite.h>
#include <Wire.h>

String Response = "";

int MAC1;    //max acceleration
int MVE1;   //max velocity, this is a test
int DIR1;    //direction What direction should the motor move (stopped(0), CCW(1) CW(2) )
int STP1;   //stop what kind of stop should be used (0(moving), 1(hard stop hold), 2 (hard stop no hold), 3(decel stop with hold), 4(decel stop without hold)
float ANG1;   //Angle the stepper should move to
#define SLAVE_ADDR 9



uStepperSLite stepper(MAC1, MVE1);

void setup() {
  // put your setup code here, to run once:
  stepper.setup();
  stepper.setMaxAcceleration(MAC1);
  stepper.setMaxVelocity(MVE1);
  stepper.setCurrent(20.0);
  //i2c slave setup 
  Wire.begin(SLAVE_ADDR);
  Wire.onReceive(receiveEvent);

  //start serial moniter for debuging
  Serial.begin(9600);
}

 /*The section below recives lines of movment code from the master and translates them to varibles 
 *  if the function recieves any of the diagnostics letters it will run the diagnostic program
 *  if the function recives movment code commands it will update its movememnt varibles
 *  acceptable varibles are 
 *  MAC1 - this sets max accelareation
 *  MVE1 - this sets max velocity
 *  DIR1 - starts movment and reverses movement
 *  STP1 - stops movement
 *  ANG1 - the angle the stepper should move to
 */
void receiveEvent()
{
Response = "";

while (Wire.available()) {
  char b = Wire.read();
  Response += b;
}

Serial.println("Recived Values: " + Response);
String vval = getValue(Response, ',', 0);
String wval = getValue(Response, ',', 1);
String xval = getValue(Response, ',', 2);
String yval = getValue(Response, ',', 3);
String zval = getValue(Response, ',', 4);


Serial.println("Max Acceleration: " + vval);
Serial.println("Max Velocity: " + wval);
Serial.println("Direction Value: " + xval);
Serial.println("Stop Value: " + yval);
Serial.println("Angle Value: " + zval);

int vvalue = vval.toInt();
MAC1 = vvalue;
int wvalue = wval.toInt();
MVE1 = wvalue;
int xvalue = xval.toInt();
DIR1 = xvalue;
int yvalue = yval.toInt();
STP1 = yvalue;
int zvalue = zval.toInt();
ANG1 = float(zvalue);

Serial.println();
//Serial.println(wvalue);
//Serial.println(xvalue);
//Serial.println(yvalue);
//Serial.println(zvalue);

//Serial.println();
//Serial.println(MAC1);
//Serial.println(MVE1);
//Serial.println(DIR1);
//Serial.println(STP1);
if(DIR1 > 0 && DIR1 < 3){
moveMotor(DIR1, MAC1, MVE1);
}
if(DIR1 == 0){
  haltMotor(STP1);
}
//if(MVE1 == "A"){
//AngleMove(DIR1, ANG1, STP1);
//}
}

//the below function does magic, all hail String getValue
String getValue(String data, char separator, int index)
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

//the bellow function defines motor movement
void moveMotor(int DIR1, int MAC1, int MVE1){                      //Run continous clockwise with currnet speed   
    if(DIR1 == 1)
    {
    stepper.enableMotor();
    stepper.setMaxAcceleration(MAC1);
    stepper.setMaxVelocity(MVE1);
    stepper.runContinous(CW);
    }
    else if(DIR1 == 2)                 //Run continous counter clockwise with current speed
    {
    stepper.enableMotor();
    stepper.setMaxAcceleration(MAC1);
    stepper.setMaxVelocity(MVE1);
    stepper.runContinous(CCW);
    }
}

// the bellow function is called to stop the motor turning
void haltMotor(int STP){
    DIR == 0;
    if(STP1 == 1)
    {
    stepper.setMaxAcceleration(0);
    stepper.setMaxVelocity(0);
    stepper.hardStop(HARD);
    }
    
    else if(STP1 == 2)
    {
    stepper.hardStop(SOFT);
    stepper.disableMotor();
    }
    
    else if(STP1 == 3)                 //Stop with deceleration and block motor
    {
    stepper.setMaxAcceleration(0);
    stepper.setMaxVelocity(0);
    stepper.softStop(HARD);
    }

    else if(STP1 == 4)                 //Stop with deceleration and block motor
    {
    stepper.softStop(SOFT);
    stepper.disableMotor();
    }
  }
/*
void AngleMove(float ANG1, int STP1, int DIR1){
  if (DIR == 1 && STP1 == 1){
  stepper.moveAngle(ANG1, HARD);
  }
  if (DIR == 1 && STP1 == 2){
  stepper.moveAngle(ANG1, SOFT);
  }
  if (DIR == 2 && STP1 == 1){
  stepper.moveAngle(ANG1 * -1, HARD);
  }
  if (DIR == 2 && STP1 == 1){
  stepper.moveAngle(ANG1 * -1, SOFT);
  }
}
*/
  void loop() {
    
}
