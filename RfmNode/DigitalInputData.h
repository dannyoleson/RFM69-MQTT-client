// DigitalInputData.h
// author: Danny Oleson - dannyoleson@gmail.com
// date: 12/28/2015
//
// for use with RfmNode.ino - https://github.com/dannyoleson/RFM69-MQTT-client/tree/master/RfmNode

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
	 int m_inputDelay;
	 long m_lastInputTime;
	 int m_lastState;
	 // crappy hack for determining whether or not this is an instance of ButtonInputData
	 bool m_isButton = false;

 public:
	 DigitalInputDataClass(int id, uint8_t pin, int inputDelay);
	 int getState();
	 bool getShouldSend();
	 void setShouldSend(bool shouldSendData);
	 long getLastInputTime();
	 int getValueToSend();
	 bool getIsButton();
	 virtual void handleTimer() { ; }
	 virtual void handleInput() { ; }
};

#endif

