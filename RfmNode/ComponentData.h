// ComponentData.h

#ifndef _COMPONENTDATA_h
#define _COMPONENTDATA_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class ComponentDataClass
{
 protected:
	 // this is a flag that can be set externally.  It should be checked in getShouldSend() functions where complex code is determining whether data need to be sent.
	 bool m_shouldSend;
	 uint8_t m_dataPin;
	 int m_valueToSend;

	 // callback types
	 typedef int(*int_call_back)(void);
	 typedef float(*flt_call_back)(void);

 public:
	long txInterval;
	int deviceId;

	ComponentDataClass(int id, uint8_t pin);
	ComponentDataClass(int id, uint8_t pin, long overrideTxInterval);
	virtual bool getShouldSend();
	void setShouldSend(bool shouldSendData);
};

extern ComponentDataClass *connectedComponents[100];
extern int connectedComponentsCount;

#endif

