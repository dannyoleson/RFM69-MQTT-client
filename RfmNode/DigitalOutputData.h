// DigitalOutputData.h

#ifndef _DIGITALOUTPUTDATA_h
#define _DIGITALOUTPUTDATA_h

#include "ComponentData.h"

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class DigitalOutputDataClass : public ComponentDataClass
{
 protected:
	 int state;

 public:
	 DigitalOutputDataClass(int id, uint8_t pin);
	 int getState();
	 void setState(int stateToSet);
	 int getValueToSend();
};

#endif

