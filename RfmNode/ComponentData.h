// ComponentData.h
// author: Danny Oleson - dannyoleson@gmail.com
// date: 12/28/2015
//
// for use with RfmNode.ino - https://github.com/dannyoleson/RFM69-MQTT-client/tree/master/RfmNode

#ifndef _COMPONENTDATA_h
#define _COMPONENTDATA_h

#include <Arduino.h>

class ComponentDataClass
{
 protected:
	 // this is a flag that can be set externally.  It should be checked in getShouldSend() functions where complex code is determining whether data need to be sent.
	 bool m_shouldSend;
	 uint8_t m_dataPin;
	 int m_valueToSend;
	 long m_lastSendTime;
	 bool m_firstRun;

	 // callback types
	 typedef int(*int_call_back)(void);
	 typedef float(*flt_call_back)(void);

 public:
	long txInterval;
	int deviceId;
	bool periodicSendEnabled;

	ComponentDataClass(int id, uint8_t pin);
	ComponentDataClass(int id, uint8_t pin, long overrideTxInterval);
	virtual bool getShouldSend();
	void setShouldSend(bool shouldSendData);
};

extern ComponentDataClass *connectedComponents[100];
extern int connectedComponentsCount;
extern long thisCycleActualMillis;
extern bool isLowPowerNode;

#endif

