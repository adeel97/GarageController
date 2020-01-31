#if 1
#include "Arduino.h"

#include"GarageHardware.h"
//#include <iostream>

//pin variables
const int closingPin = D0;
const int openingPin = D1;
const int lightPin = A4;
const int buttonPin = D4;
const int closedPin = D5;
const int openPin = D6;
const int fault = D3;
/**
 * Setup the door hardware (all I/O should be configured here)
 *
 * This routine should be called only once from setup()
 */
void setupHardware() {
  Serial.begin(9600);
  pinMode(closingPin,OUTPUT);//door closing
  pinMode(openingPin,OUTPUT);//door opening
  pinMode(lightPin,OUTPUT);//light!
  pinMode(buttonPin,INPUT); //door open/close button
  pinMode(closedPin,INPUT); //door fully closed
  pinMode(openPin,INPUT); //door fully open
  pinMode(fault,INPUT); //fault

}

/**
 * Return true if the door open/close button is pressed
 *
 * Note: this is directly based on hardware.  No debouncing or
 *       other processing is performed.
 *
 * return  true if buttons is currently pressed, false otherwise
 */
boolean isButtonPressed() {
  if(digitalRead(buttonPin)==HIGH){
    return true;
  }
  else{
    return false;
  }
}

/**
 * Return true if the door is fully closed
 *
 * Note: This is directly based on hardware.  No debouncing or
 *       other processing is performed.
 *
 * return  true if the door is completely closed, false otherwise
 */
boolean isDoorFullyClosed() {
  if(digitalRead(closedPin)==HIGH){
    return true;
  }
  else{
    return false;
  }
}

/**
 * Return true if the door has experienced a fault
 *
 * Note: This is directly based on hardware.  No debouncing or
 *       other processing is performed.
 *
 * return  true if the door is has experienced a fault
 */
boolean isFaultActive() {
  if(digitalRead(fault)==HIGH){
    return true;
  }
  else{
    return false;
  }
}

/**
 * Return true if the door is fully open
 *
 * Note: This is directly based on hardware.  No debouncing or
 *       other processing is performed.
 *
 * return  true if the door is completely open, false otherwise
 */
boolean isDoorFullyOpen() {
  if(digitalRead(openPin)==HIGH){
    return true;
  }
  else{
    return false;
  }
}

/**
 * This function will start the motor moving in a direction that opens the door.
 *
 * Note: This is a non-blocking function.  It will return immediately
 *       and the motor will continue to opperate until stopped or reversed.
 *
 * return void
 */
void startMotorOpening() {
  digitalWrite(openingPin,HIGH);
}

/**
 * This function will start the motor moving in a direction closes the door.
 *
 * Note: This is a non-blocking function.  It will return immediately
 *       and the motor will continue to opperate until stopped or reversed.
 *
 * return void
 */
void startMotorClosing() {
  digitalWrite(closingPin,HIGH);
}

/**
 * This function will stop all motor movement.
 *
 * Note: This is a non-blocking function.  It will return immediately.
 *
 * return void
 */
void stopMotor() {
  digitalWrite(openingPin,LOW);
  digitalWrite(closingPin,LOW);
}

/**
 * This function will control the state of the light on the opener.
 *
 * Parameter: on: true indicates the light should enter the "on" state;
 *                false indicates the light should enter the "off" state
 *
 * Note: This is a non-blocking function.  It will return immediately.
 *
 * return void
 */
void setLight(boolean on) {
  if(on){
    analogWrite(lightPin,255);
  }
  else{
    analogWrite(lightPin,0);
  }
}

/**
 * This function will control the state of the light on the opener.
 *
 * Parameter: cycle (0-100).  0 indicates completely Off, 100 indicates completely on.
 *            intermediate values are the duty cycle (as a percent)
 *
 * Note: This is a non-blocking function.  It will return immediately.
 *
 * return void
 */
void setLightPWM(int cyclePct) {
  analogWrite(lightPin,(cyclePct*255)/100);
}

/**
 * This function will send a debugging message.
 *
 * Parameter: message. The message (no more tha 200 bytes)
 *
 * Note: This is a non-blocking function.  It will return immediately.
 *
 * return void
 */
void sendDebug(String message) {
  Serial.println(message);
}

#endif
