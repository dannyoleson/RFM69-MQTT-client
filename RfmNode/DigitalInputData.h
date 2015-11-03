// DigitalInputData.h

#ifndef _DIGITALINPUTDATA_h
#define _DIGITALINPUTDATA_h

#include "ComponentData.h"

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class DigitalInputDataClass : public ComponentDataClass
{
 protected:
	 int inputDelay;
	 long lastInputTime;
	 int lastState;
	 int currentState;

 public:
	 DigitalInputDataClass(int id, uint8_t pin, int delay);
	 int getState();
	 bool getShouldSend();
	 void setShouldSend(bool shouldSendData);
	 long getLastInputTime();
	 int getValueToSend();
};

#endif

