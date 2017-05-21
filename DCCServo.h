#ifndef _DCCSERVO_H
#define _DCCSERVO_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Servo.h>

#define SERVO_NONE		0x00
#define SERVO_INITL1		0x01
#define SERVO_INITL2		0x02
#define SERVO_INITMID		0x03

#define SERVO_ABSOLUTE		0x04
#define SERVO_BOUNCE_L1		0x08
#define SERVO_BOUNCE_L2		0x10
#define SERVO_AUTO_REVERSE	0x20
#define SERVO_REVERSE		0x80

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
    unsigned int
	     flags;	 // Configuration flags
    int      bounce_ang; // First bounce angle
    int      bounce_step;// how far along in the bounce are we
    int      blimit;
    int      bclockwise;
    boolean  bounced;
    boolean  bouncing;
    int      bouncepoint;
    int	     reported;
  public:
    DCCServo(int, int, int, unsigned int, unsigned int flags = SERVO_INITL1);
    void	loop();
    void	setSpeed(int, boolean);
    void	setActive(boolean);
    void	setStart(int);
    void	setEnd(int);
    void	setTravelTime(int);
    void	setFlags(int);
    void        setBounceAngle(int);
    void	setPosition(int);
    void	setAngle(int);
    void	writeTenths(int);
    boolean	isAbsolute();
    int		getAngle();
};

extern void notifyServoPosition(DCCServo *, int) __attribute__ ((weak));
#endif
