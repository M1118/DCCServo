#ifndef _DCCSERVO_H
#define _DCCSERVO_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Servo.h>

class DCCServo {
  private:
    Servo    servo;      // The encapsulted servo
    int      limit1;     // Limit of travel in one direction
    int      limit2;     // Limit of travel in the other direction
    int      tlimit1;    // Limit of travel in one directionin tenths
    int      tlimit2;    // Limit of travel in the other direction in tenths
    unsigned long    
             travelTime; // Total travel time between two limits in milliseconds
    unsigned long     
             interval;   // Time between steps of 1 degree in milliseconds
    int      pin;        // The control pin for the servo
    boolean  active;     // Is the servo active?
    boolean  clockwise;  // Current direction is clockwise
    int      percentage; // Current speed percentage
    boolean  moving;     // Servo is moving
    int      angle;      // Current angle of the servo
    unsigned long
             refresh;    // Next refresh time mills()
  public:
    DCCServo(int, int, int, unsigned int);
    void loop();
    void setSpeed(int, boolean);
    void setActive(boolean);
    void setStart(int);
    void setEnd(int);
    void setTravelTime(int);
    void writeTenths(int);
};

#endif
