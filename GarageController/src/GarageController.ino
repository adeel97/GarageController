/*
* Project GarageController
* Description: This app controls a garage.
* Author: Zach Perlman and Adeel Shaikh
* Date: April 16, 2018
*/

#include "GarageHardware.h"
//variables
bool wasOpening = false; //these boolean variables will be used later to check if the
bool wasClosing = false; //stopped state has been reached from open or closed
bool prevState1 = false; //variable
bool fadeDone = false; //variable
bool firstFade = true; //variable
bool f = false; //variable
bool pressed = false; //variable
bool publishDone = false;
//bool lightStatus = true; //variable
int next; //variable
int light; //variable
int autoClose = 10000; //variable
int delta2=0; //variable
int delta=0; //variable
int autoOn=0; //variable
String data; //variable
String data2; //variable
String data3; //variable
int interval = 4000; //variable
int looper; //variable
int lightTimer = 5000; //variable
int maxLight = 100; //variable
int lightON = 0; //variable

enum State {
  closed,  //door is closed
  opening, //door opening
  open, //door opening
  closing, //door closing
  stopped, //door has stopped moving
  fault, //fault has been detected
  waiting //to wait at the begining
};

State nextState(State state);
State doorState = waiting;

const String topic = "GarageThing";
const String topic2 = "autoCloseThing";
const String topic3 = "soToggle";


void myHandler(String event, String data)
{
  Serial.print(event);
  Serial.print(", data: ");
  Serial.print(data);
  Serial.println((data));
  if(data=="1"){
    buttonpressed("arg");
  }
}
void myHandler2(String event, String data)
{
  lightSTATUS(data);
}

void setup() {
  Particle.subscribe("REMOTESTREAM", myHandler,MY_DEVICES);
  Particle.subscribe("lightSTREAM", myHandler2,MY_DEVICES);
  setupHardware();
  publishState();
  Particle.function("button", buttonpressed);
  Particle.function("autoTime", closeTime);
  Particle.function("autoToggle", toggle);
  //Particle.function("autoToggleON", toggleON);
  Particle.function("INITUI", initUI);
  Particle.function("lightTimer", lightTime);
  Particle.function("maxLight", maxLighter);
  Particle.function("lightStatus", lightSTATUS);

  Serial.println("timer: ");
  Serial.println(autoClose);
  looper = millis();
}

int lightSTATUS(String command){
  if (command.toInt()==1){
    Serial.println("light button pressed");
    setLightPWM(maxLight);
    lightON = 1;
    fadeDone = false;
    firstFade = true;
    publishState();
    publishDone = true;
    return 1;
  }
  else{
    Serial.println("light off pressed");
    lightON = 0;
    light = 0;
    setLight(false);
    publishState();
    publishDone = true;
    return 0;
  }
}

int lightTime(String command){
  Serial.println("button pressed");
  lightTimer = (command.toInt())*1000;
  delta = millis()+lightTimer;
  publishState();
  return 0;
}

int maxLighter(String command){
  Serial.print("maxlight: ");
  maxLight = command.toInt();
  light = maxLight;
  if (lightON){
    setLightPWM(maxLight);
  }

  Serial.println(maxLight);
  publishState();
  return 0;
}

int initUI(String command){
  Serial.println("UI initialized");
  publishState();
  return 0;
}

int buttonpressed(String command){
  Serial.println("button pressed");
  pressed = true;
  publishState();
  return 0;
}

int closeTime(String myTime){
  autoClose = (myTime.toInt())*1000;
  delta2 = millis()+autoClose;
  Serial.println(autoClose);
  publishState();
  return 0;
}

int toggle(String myNum){
  Serial.println(myNum);
  Serial.println("disabled autoON");
  autoOn=myNum.toInt();
  publishState();
  return 0;
}

int toggleON(String myNum){
  Serial.println(myNum);
  Serial.println("enabled autoON");
  autoOn=1;
  publishState();
  return 1;
}

void loop() {
  doorState = nextState(doorState);
}

int publishState() {
  data = "{";
  data += "\"lightStatus\":";
  data += lightON;
  data += ", ";
  data += "\"doorState\":";
  data += doorState;
  data += ", ";
  data += "\"lightTimer\":";
  data += lightTimer;
  data += ", ";
  data += "\"maxLight\":";
  data += maxLight;
  data += ", ";
  data += "\"autoClose\":";
  data += autoClose;
  data += ", ";
  data += "\"autoOn\":";
  data += autoOn;
  data += ", ";
  data += "\"maxLight\":";
  data += maxLight;
  data += "}";

  // data = doorState;
  // data2 = autoClose;
  // data3 = autoOn;

  Serial.println("Current State:");
  Serial.println(data);
  Particle.publish(topic, data, 60, PRIVATE);
  //Particle.publish(topic2, data2, 60, PRIVATE);
  //Particle.publish(topic3, data3, 60, PRIVATE);
  return 0;
}

bool debounce() {
  int threshold = 100;
  if(isButtonPressed()&&!prevState1){
    int count = millis();
    int done = count+threshold;
    while(isButtonPressed()){
      if(millis()>=done){
        prevState1=true;
        return true;
      }
    }
  }

  else if(!isButtonPressed()){
    prevState1=false;
    return false;
    //pressed = false;
  }

  else{
    return false;
    //pressed = false;
  }
}

void fade() {
  fadeDone = false;

  //pressed = false;
  //original fade function
  int interval = 5000/maxLight;
  if(firstFade){
    next = millis() - 1;
    light = maxLight;
    firstFade = false;
    fadeDone = false;
    delta = millis()+lightTimer-1;
  }
  if(millis()>delta){
    if((light>0)&&(millis()>=next)){
      //Serial.println(light);
      setLightPWM(light);
      light=light-1;
      next = millis() + interval;
    }
    if(light==0) {
      setLight(false);
      fadeDone=true;
      lightON = 0;
      publishState();
    }

  }
}

State nextState(State state) {
  switch(state) {
    case closed:

    stopMotor(); //stop the motor
    if (!fadeDone){
      lightON = 1;
      fade(); //make the light fade out
      if (!publishDone){
        publishState();
        publishDone = true;
      }
      //fade(); //make the light fade out
    }
    //  fade(); //make the light fade out
    //to check if open/close button is pressed
    if(debounce()||pressed) {
      pressed = false;
      state = opening; //if door is closed and button pressed, start opening the door
      doorState = opening;
      publishDone = false;
    }
    break;

    case opening:
    if (!publishDone){
      lightON = 1;
      setLightPWM(maxLight);
      startMotorOpening();
      publishState();
      publishDone = true;
    }

    // setLight(true);
    // lightON = 1;
    wasOpening = true;
    wasClosing = false;
    //publishState();
    //to check if open/close button is pressed
    if (debounce()||pressed) {
      pressed = false;
      fadeDone=false;
      firstFade=true;
      state = stopped; //if door was opening and button is pressed, stop
      doorState = stopped;
      publishDone = false;
      //publishState();
    }
    //to check if open switch button is pressed
    if(isDoorFullyOpen()) {
      fadeDone=false;
      firstFade=true;
      state = open; //if door is fully open, go to open state
      doorState = open;
      publishDone = false;
      //publishState();
    }
    //to check if fault button is pressed
    if(isFaultActive()) {
      fadeDone=false;
      firstFade=true;
      state = fault; //if door is closed and fault detected, go to fault state
      doorState = fault;
      publishDone = false;
      //publishState();
    }
    break;

    case open:
    //Serial.println("open");
    stopMotor(); //stop the motor
    if (!fadeDone){
      lightON = 1;
      fade(); //make the light fade out
      if (!publishDone){
        publishState();
        publishDone = true;
      }
    //  fade(); //make the light fade out
    }
    //to check if open/close button is pressed
    //Serial.println(pressed);
    if(debounce()||pressed) {
      pressed = false;
      state = closing; //if door is closed and button pressed, start opening the door
      doorState = closing;
      delta2=0;
      publishDone = false;
      //publishState();
    }
    if(autoOn==1){
      if(delta2==0){
        delta2 = millis()+autoClose;
      }
      if(millis()>delta2){
        pressed = false;
        state = closing;
        doorState = closing;
        setLightPWM(maxLight);
        lightON = 1;
        //publishState();
        delta2=0;
        publishDone = false;
      }
    }
    break;

    case closing:
    if (!publishDone){
      setLightPWM(maxLight);
      lightON = 1;
      publishState();
      startMotorClosing();
      publishDone = true;
    }


    wasOpening = false;
    wasClosing = true;
    //publishState();
    //to check if open/close button is pressed
    if(debounce()||pressed) {
      pressed = false;
      fadeDone=false;
      firstFade=true;
      state = stopped; //if door is closing and button pressed, stop
      doorState = stopped;
      publishDone = false;
      //publishState();
    }
    //to check if close switch button is pressed
    if(isDoorFullyClosed()) {
      fadeDone=false;
      firstFade=true;
      state = closed; //if door is fully closed, go to closed state
      doorState = closed;
      publishDone = false;
      //publishState();
    }
    //to check if fault button is pressed
    if(isFaultActive()) {
      fadeDone=false;
      firstFade=true;
      state = fault; //if door is closed and fault detected, go to fault state
      doorState = fault;
      publishDone = false;
      //publishState();
    }
    break;

    case stopped:
    stopMotor(); //stop the motor

    if (!publishDone){
      publishState();
      publishDone = true;
    }

    if (!fadeDone){
      lightON = 1;
      fade();
      if (!publishDone){
        publishState();
        publishDone = true;
      }

    }
    if(wasOpening==true) {
      if(debounce()||pressed) {
        pressed = false;
        state = closing; //if door is closing and button pressed, stop
        doorState = closing;
        publishDone = false;
        //publishState();
      }
    }
    else if(wasClosing==true) {
      if(debounce()||pressed) {
        pressed = false;
        state = opening; //if door is closing and button pressed, stop
        doorState = opening;
        publishDone = false;
        //publishState();
      }
    }
    break;

    case fault:
    if (!publishDone){
      publishState();
      stopMotor(); //stop the motor
      publishDone = true;
    }

    if(!f){
      sendDebug("fault");
      f=true;
    }
    else{
      f=false;
    }
    //make the light fade out
    if (!fadeDone){
      lightON = 1;
      fade();
      if (!publishDone){
        publishState();
        publishDone = true;
      }

    }
    if(wasOpening==true) {
      if(debounce()||pressed) {
        pressed = false;
        state = closing; //if door is closing and button pressed, stop
        doorState = closing;
        publishDone = false;
        //publishState();
      }
    }
    else if(wasClosing==true) {
      if(debounce()||pressed) {
        pressed = false;
        state = opening; //if door is closing and button pressed, stop
        doorState = opening;
        publishDone = false;
        //publishState();
      }
    }
    break;

    case waiting:
    if (!publishDone){
      publishState();
      publishDone = true;
    }
    if(isDoorFullyClosed()) {
      state = closed;
      doorState = closed;
      publishDone = false;
      nextState(closed);
      //publishState();
    }
    else if(isDoorFullyOpen()) {
      state = open;
      doorState = open;
      publishDone = false;
      nextState(open);
      //publishState();
    }
    break;

  }
  return state;
}
