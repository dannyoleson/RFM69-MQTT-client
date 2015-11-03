// RealInputData.h

#ifndef _REALINPUTDATA_h
#define _REALINPUTDATA_h

#include "ComponentData.h"

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class RealInputDataClass : public ComponentDataClass
{
 protected:
	 long lastTxTime;
	 float lastReading;
	 flt_call_back getValueCB;
	 bool firstArg;
	 bool secondArg;

 public:
	bool shouldSend();
	float getValueToSend();

	RealInputDataClass(int id, int pin, flt_call_back getValueCallBack);
};

#endif

