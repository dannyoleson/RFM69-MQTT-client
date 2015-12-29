// AnalogOutputData.h
// author: Danny Oleson - dannyoleson@gmail.com
// date: 12/28/2015
//
// for use with RfmNode.ino - https://github.com/dannyoleson/RFM69-MQTT-client/tree/master/RfmNode

#ifndef _ANALOGOUTPUTDATA_h
#define _ANALOGOUTPUTDATA_h

#include "ComponentData.h"

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class AnalogOutputDataClass : public ComponentDataClass
{
protected:
	int state;

public:
	AnalogOutputDataClass(int id, uint8_t pin);	
	void setState(int stateToSet);
	int getValueToSend();
};

#endif

