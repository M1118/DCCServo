#include <DCCServo.h>

DCCServo::DCCServo(int pin, int limit1, int limit2, unsigned int travelTime, unsigned int flags)
{
unsigned long travelTimeMS;

  this->pin = pin;
  this->limit1 = limit1;
  this->limit2 = limit2;
  this->tlimit1 = limit1 * 10;
  this->tlimit2 = limit2 * 10;
  travelTimeMS = travelTime * 1000L;
  this->travelTime = travelTimeMS;
  this->angle = tlimit1;
  this->interval = this->travelTime / (this->tlimit2 - this->tlimit1);
  this->refresh = millis() + this->interval;
  this->moving = false;
  this->active = false;
  this->clockwise = true;
  this->flags = flags;
  switch (flags & (SERVO_INITL1|SERVO_INITL2|SERVO_INITMID))
  {
  case SERVO_INITL1:
    this->servo.attach(this->pin);
    this->servo.write(this->limit1);
    break;
  case SERVO_INITL2:
    this->servo.attach(this->pin);
    this->servo.write(this->limit2);
    break;
  case SERVO_INITMID:
    this->servo.attach(this->pin);
    this->servo.write(this->limit1 + (this->limit2 - this->limit1) / 2);
    break;
  }
  this->bouncing = false;
  this->bounced = false;
  this->bounce_step = 0;
#if DEBUG
  Serial.print("Servo on pin ");
  Serial.println(pin);
  Serial.print("TravelTime is ");
  Serial.println(this->travelTime);
  Serial.print("Limit1 is ");
  Serial.println(this->limit1);
  Serial.print("Limit2 is ");
  Serial.println(this->limit2);
  Serial.print("Interval is ");
  Serial.println(this->interval);
  Serial.println("-------------------------------");
#endif
}

void DCCServo::loop()
{
  int  newangle;
  boolean clockwise;

  if (this->flags & SERVO_ABSOLUTE)
    return;
  
  if (this->refresh > millis())
    return;
  
  if (this->active == false || this->moving == false)
  {
    this->refresh = millis() + this->interval;
    return;
  }

  clockwise = this->clockwise;
  if (this->flags & SERVO_REVERSE)
    clockwise = ! clockwise;

  newangle = this->angle;
  if (this->bouncing)
  {
    if (this->bclockwise && this->angle < this->blimit)
    {
      newangle = this->angle + 1;
    }
    else if (this->bclockwise == false && this->angle > this->bouncepoint)
    {
      newangle = this->angle - 1;
    }
    else if (this->angle == this->bouncepoint)
    {
      this->bounce_step++;
      int bounce_angle;
      bounce_angle = 10 * (this->bounce_ang >> (this->bounce_step / 2));
      if (bounce_angle == 0)
      {
        this->bouncing = false;
        this->bounced = true;
        newangle = this->tlimit2;
      }
      else
      {
	if (this->blimit == this->tlimit2)
        	this->bouncepoint = this->tlimit2 - (10 * bounce_angle);
	else
		this->blimit = this->tlimit1 + (10 * bounce_angle);
        this->bclockwise = ! this->bclockwise;
      }
    }
    else
    {
      this->bounce_step++;
      int bounce_angle;
      bounce_angle = 10 * (this->bounce_ang >> (this->bounce_step / 2));
      if (bounce_angle == 0)
      {
        this->bouncing = false;
        this->bounced = true;
        newangle = this->tlimit1;
      }
      else
      {
	if (this->blimit == this->tlimit2)
        	this->bouncepoint = this->tlimit2 - (10 * bounce_angle);
	else
		this->blimit = this->tlimit1 + (10 * bounce_angle);
        this->bclockwise = ! this->bclockwise;
      }
    }
  }
  else
  {
    if (clockwise && this->angle < this->tlimit2)
    {
      newangle = this->angle + 1;
      this->bounced = false;
    }
    else if (clockwise == false && this->angle > this->tlimit1)
    {
      newangle = this->angle - 1;
      this->bounced = false;
    }
    else if ((this->flags & SERVO_BOUNCE_L1) != 0 && clockwise == false
            && this->bounced == false && this->bounce_ang != 0)
    {
      this->bounce_step = 1;
      this->bouncing = true;
      this->bouncepoint = this->angle;
      this->blimit = this->tlimit1 + (10 * this->bounce_ang);
      this->bclockwise = true;
    }
    else if ((this->flags & SERVO_BOUNCE_L2) != 0 && clockwise
           && this->bounced == false && this->bounce_ang != 0)
    {
      this->bounce_step = 1;
      this->bouncing = true;
      this->blimit = this->angle;
      this->bouncepoint = this->tlimit2 - (10 * this->bounce_ang);
      this->bclockwise = false;
    }
    else if (this->flags & SERVO_AUTO_REVERSE)
      this->flags = this->flags ^ SERVO_REVERSE;
    else
      this->moving = false;
  }


  if (newangle != this->angle)
  {
    this->writeTenths(newangle);
    this->angle = newangle;
  }
  if (this->angle / 10 != this->reported)
  {
    this->reported = this->angle / 10;
    if (notifyServoPosition)
        notifyServoPosition(this, this->reported);
  }
  unsigned long delta = (this->interval * 100) / this->percentage;
  this->refresh = millis() + delta;
 
}

void DCCServo::setSpeed(int percentage, boolean clockwise)
{
  this->percentage = percentage;
  this->clockwise = clockwise;
  this->moving = (percentage > 0);
}

void DCCServo::setActive(boolean active)
{
  if ((active == true) && (this->active == false))
  {
    this->servo.attach(this->pin);
    this->refresh = millis() + ((this->interval * 100) / this->percentage);
  }
  else if ((active == false) && (this->active == true))
  {
    this->servo.detach();
  }
  this->active = active;
}

void DCCServo::setStart(int start)
{
  if (start < 0)
    start = 0;
  else if (start > 180)
    start = 180;
  this->limit1 = start;
  this->tlimit1 = start * 10;
  this->interval = this->travelTime / (this->tlimit2 - this->tlimit1);
}

void DCCServo::setEnd(int angle)
{
  if (angle < 0)
    angle = 0;
  else if (angle > 180)
    angle = 180;
  this->limit2 = angle;
  this->tlimit2 = angle * 10;
  this->interval = this->travelTime / (this->tlimit2 - this->tlimit1);
}

void DCCServo::setFlags(int flags)
{
  this->flags = (unsigned int)flags;
}

void DCCServo::setBounceAngle(int angle)
{
  this->bounce_ang = angle;
}

void DCCServo::setPosition(int percentage)
{
unsigned long range = this->tlimit2 - this->tlimit1;
unsigned long tenth = (percentage * range) / 100;

	tenth += this->tlimit1;
	if (this->active)
	{
		this->angle = tenth;
		this->writeTenths((int)tenth);
	}
}		

void DCCServo::setAngle(int angle)
{
unsigned long tenth = angle * 10;

	this->angle = tenth;
	if (this->active)
	{
		this->writeTenths((int)tenth);
	}
}		

int DCCServo::getAngle()
{
	return this->angle / 10;
}

void DCCServo::setTravelTime(int time)
{
  this->travelTime = time * 1000;
  this->interval = this->travelTime / (this->tlimit2 - this->tlimit1);
}

boolean DCCServo::isAbsolute()
{
  if (this->flags & SERVO_ABSOLUTE)
  	return true;
  return false;
}

#define SERVO_MIN() (MIN_PULSE_WIDTH)  // minimum value in uS for this servo
#define SERVO_MAX() (MAX_PULSE_WIDTH)  // maximum value in uS for this servo 

void DCCServo::writeTenths(int tenth)
{
	this->servo.writeMicroseconds(map(tenth, 0, 1800,
					SERVO_MIN(),  SERVO_MAX()));
}
