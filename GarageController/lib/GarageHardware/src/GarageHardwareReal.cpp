#if 0
#include "Arduino.h"

#include"GarageHardware.h"

#define gc Serial1   // Garage Controller is connected via Serial port 1

//#define DEBUG_G 1

#ifdef DEBUG_G
#define debug_print(a)  Serial.print(a)
#define debug_println(a)  Serial.println(a)
#else
#define debug_print(a)  ;
#define debug_println(a)  ;
#endif

// Forward declarations
static void rx();
static void sendMessage(byte type, byte *data, byte dataLength);
static void sendError(char *message);


// Buffer for incoming messages
static byte buffer[255];  // Should never exceed ~3 bytes on Photon; 255 on APM
static byte bufferIndex = 0;
static bool inMessage = false;
static Timer rxCheck(50,rx);
static volatile byte statusByte = 0; // No bits set
static const int baud = 57600;
   // Bit 0 => Button
   // Bit 1 => Fully open
   // Bit 2 => Fully closed
   // Bit 3 => Fault
static const byte startByte = 0x55;

// Timing
static unsigned long buttonPressStart=0;
static unsigned long openedSwitchStart=0;
static unsigned long closedSwitchStart=0;
static unsigned long faultStart=0;

// Status Byte bit masks
enum {
    SB_BUTTON_MASK = 0x01,
    SB_DOOR_OPENED_MASK = 0x02,
    SB_DOOR_CLOSED_MASK = 0x04,
    SB_FAULT_MASK = 0x08
};

// Enumeration of meaning of bytes within messages
enum {
    COMM_SIZE_BYTE=0,
    COMM_TYPE_BYTE=1,
    COMM_DATA_START=2
};

static enum {
    MOTOR_OPENING,
    MOTOR_CLOSING,
    MOTOR_STOPPED
} motorState = MOTOR_STOPPED;

enum {
    MSG_STATUS = 1,
    MSG_MOTOR = 2,
    MSG_LIGHT = 3,
    MSG_ERROR = 4
};

enum {
    MOTOR_STOP = 0,
    MOTOR_OPEN = 1,
    MOTOR_CLOSE = 2
};

/*
  rx() : Retrieve any available bytes and process them
         If a complete, valid message is received, dispatch it as needed.
 */
static void rx() {
  unsigned long now = millis();
  while(gc.available()) {
    byte b = gc.read();
    if(!inMessage && b == startByte) {
      //debug_println("Start of message");
      inMessage = true;
      bufferIndex = 0;
    } else if(inMessage) {
      buffer[bufferIndex] = b;
      bufferIndex++;
      // If we have a message size and all the bytes...
      if(bufferIndex>COMM_SIZE_BYTE && bufferIndex == buffer[COMM_SIZE_BYTE]) {
        //debug_println("End of message");

        // Message complete.  Check the checksum and if good, Process the message
        byte total = 0;
        byte newStatus = 0;

        // Add all bytes from size to last data (inclusive)
        // Only ignores start (0x55) and length)
        for(int i=1;i<bufferIndex-1;i++) {
          total += buffer[i];
        }
        if(total == buffer[bufferIndex-1]) {
          //debug_println("Good Checksum");

          // Good! Process it
          switch(buffer[COMM_TYPE_BYTE]) {  // Switch on type
            case MSG_STATUS: // Status byte (to Photon; Never to APM)
               newStatus = buffer[COMM_DATA_START];
               debug_print("Status Byte: ");
               debug_println(newStatus);
               // Button Press
               if(newStatus & SB_BUTTON_MASK) {
                   buttonPressStart = now;
               }
               // Open Switch
               if(newStatus & SB_DOOR_OPENED_MASK) {
                   openedSwitchStart = now;
                   debug_print("Open switch hit: ");
                   debug_println(now);
               }

               // Close Switch
               if(newStatus & SB_DOOR_CLOSED_MASK) {
                   closedSwitchStart = now;
               }
               // Fault
               if(newStatus & SB_FAULT_MASK) {
                   faultStart = now;
               }
               statusByte = newStatus;
            break;

            case MSG_MOTOR: // Motor control (to APM; Never to Photon)
               debug_print("Motor Control: ");
               debug_println(buffer[COMM_DATA_START]);
               switch(buffer[COMM_DATA_START]) {
                case MOTOR_STOP:  // Stop
                break;
                case MOTOR_OPEN:  // Open
                break;
                case MOTOR_CLOSE: // Close
                break;
               }
            break;

            case MSG_LIGHT: // Light (to APM; Never to Photon)
               debug_print("Light Control: ");
               debug_println(buffer[COMM_DATA_START]);
            break;

            case MSG_ERROR: // Error (to APM; Never to Photon)
               debug_print("Error: ");
               // Add null and print
               buffer[bufferIndex-1]=0;
               debug_println((char*)(buffer+COMM_DATA_START));
            break;

            default:
               debug_println("Undefined Message type");
          }
        }

        // Either way, reset
        inMessage = false;
        bufferIndex = 0;
      }
    }
  }
}

/*
  Send the designaged message type & data.  Builds length & checksum.
*/
static void sendMessage(byte type, byte *data, byte dataLength) {
  //debug_print("Sending message ");
  //debug_println(type);
  byte checkSum = 0;
  byte toSend;
  gc.write(startByte);      // Start byte
  gc.write(dataLength+3);  // Length of data + length byte + type + checksum
  gc.write(type);      // Write the type
  checkSum += type;
  for(int i=0;i<dataLength;i++) {
    gc.write(data[i]);
    checkSum += data[i];
  }
  gc.write(checkSum);
}


/**
 * Setup the door hardware (all I/O should be configured here)
 *
 * This routine should be called only once from setup()
 */
void setupHardware() {
    // Setup communications
    gc.begin(baud);
    rxCheck.start();
}

/**
 * Return true if the door open/close button is pressed
 *
 * Note: this is directly based on hardware.  No debouncing or
 *       other processing is performed.
 *
 * return  true if buttons is currently pressed, false otherwise
 */
bool isButtonPressed() {
  // TODO: Your code to simulate a button press
  rx();  // Check for any updates
  return (statusByte & SB_BUTTON_MASK) ? true : false;
}

/**
 * Return true if the door is fully closed
 *
 * Note: This is directly based on hardware.  No debouncing or
 *       other processing is performed.
 *
 * return  true if the door is completely closed, false otherwise
 */
bool isDoorFullyClosed() {
  rx();  // Check for any updates
  return (statusByte & SB_DOOR_CLOSED_MASK) ? true : false;
}

/**
 * Return true if the door has experienced a fault
 *
 * Note: This is directly based on hardware.  No debouncing or
 *       other processing is performed.
 *
 * return  true if the door is has experienced a fault
 */
bool isFaultActive() {
  rx();  // Check for any updates
  return (statusByte & SB_FAULT_MASK) ? true : false;
}

/**
 * Return true if the door is fully open
 *
 * Note: This is directly based on hardware.  No debouncing or
 *       other processing is performed.
 *
 * return  true if the door is completely open, false otherwise
 */
bool isDoorFullyOpen() {
  rx();  // Check for any updates
  return (statusByte & SB_DOOR_OPENED_MASK) ? true : false;
}


static void buttonTimingCheck() {
  // Check time since button press
  unsigned long delta = millis() - buttonPressStart;
  // If not moving and within 1.5s of button, assume this is starting motion & check timing
  if(motorState==MOTOR_STOPPED && delta<1500) {
      if(delta<100)
        sendError("Possible Error: Door motion less than 100ms after button");
      if(delta>150)
        sendError("Possible Error: Door motion more than 150ms after button");
  }
  // If moving and more than 200ms, assume that the door was supposed to stop sooner
  if(motorState != MOTOR_STOPPED && delta>200) {
        sendError("Possible Error: Door motion stopped more than 200ms after button");
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
  if(motorState != MOTOR_OPENING) {
    buttonTimingCheck();
    byte data = MOTOR_OPEN; // Code to start opening
    sendMessage(MSG_MOTOR, &data, 1);
    motorState = MOTOR_OPENING;
  }
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
  if(motorState != MOTOR_CLOSING) {
    buttonTimingCheck();
    byte data = MOTOR_CLOSE; // Code to start opening
    sendMessage(MSG_MOTOR, &data, 1);
    motorState = MOTOR_CLOSING;
  }
}

/**
 * This function will stop all motor movement.
 *
 * Note: This is a non-blocking function.  It will return immediately.
 *
 * return void
 */
void stopMotor() {
  if(motorState != MOTOR_STOPPED) {
    unsigned long now = millis();
    unsigned long delta = now - buttonPressStart;
    // If it's been between 200ms and 1.5s since button was press, it's possible that it was more than 200ms to stop
    if(delta>200 && delta<1500) {
        sendError("Possible Error: Door motion stopped more than 200ms after button");
    }

    // If opening, switch is hit, and too much time
    delta = now - openedSwitchStart;
    if(motorState == MOTOR_OPENING && (statusByte & SB_DOOR_OPENED_MASK) && delta>200) {
      debug_print("Stopped at: ");
      debug_println(now);
        sendError("Possible Error: Door motion stopped more than 200ms after hitting open switch");
    }

    // If closing, switch is hit, and too much time
    delta = now - closedSwitchStart;
    if(motorState == MOTOR_CLOSING && (statusByte & SB_DOOR_CLOSED_MASK) && delta>200) {
        sendError("Possible Error: Door motion stopped more than 200ms after hitting close switch");
    }

    // If fault and too much time
    delta = now - faultStart;
    if(motorState != MOTOR_STOPPED && (statusByte & SB_FAULT_MASK) && delta>200) {
        sendError("Possible Error: Door motion stopped more than 200ms after fault");
    }

    byte data = MOTOR_STOP; // Code to start opening
    sendMessage(MSG_MOTOR, &data, 1);
    motorState = MOTOR_STOPPED;
  }
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
  setLightPWM(on ? 100 : 0);
}


/**
 * This function will send a text error message.
 *
 * Parameter: message --- the message to send.
 *
 * Note: This is a non-blocking function.  It will return immediately.
 *
 * return void
 */
void sendError(char *message) {
    sendMessage(MSG_ERROR, (byte*)message, strlen(message));
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
  byte data;
  if(cyclePct == 0)
    data = 0;
  else if(cyclePct == 100)
    data = 255;
  else if(cyclePct>100)
    data = 0;
  else
    data = cyclePct/100.0*255;
  sendMessage(MSG_LIGHT, &data, 1);
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
    char buffer[209];
    strncpy(buffer, "DEBUG: ",7);
    int messageLen = min(200, message.length());
    strncpy(buffer+7, message.c_str(), messageLen);
    buffer[messageLen+7+1] = '\0';
    sendError(buffer);
}

#endif
