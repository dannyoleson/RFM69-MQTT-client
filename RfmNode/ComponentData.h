// ComponentData.h

#ifndef _COMPONENTDATA_h
#define _COMPONENTDATA_h

#include <DHT.h>

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class ComponentDataClass
{
 protected:
	 bool shouldSend;
	 int txInterval;

	 // callback types
	 typedef int(*int_call_back)(void);
	 typedef float(*flt_call_back)(void);

 public:
	int deviceId;
	uint8_t dataPin;
	int valueToSend;

	ComponentDataClass(int id, uint8_t pin);
	bool getShouldSend();
	void setShouldSend(bool shouldSendData);
};

extern ComponentDataClass *connectedComponents[100];
extern int connectedComponentsCount;

#endif

