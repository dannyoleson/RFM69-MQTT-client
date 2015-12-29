// RealInputData.h
// author: Danny Oleson - dannyoleson@gmail.com
// date: 12/28/2015
//
// for use with RfmNode.ino - https://github.com/dannyoleson/RFM69-MQTT-client/tree/master/RfmNode

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
	 flt_call_back getValueCB;
	 bool firstArg;
	 bool secondArg;

 public:
	bool getShouldSend();
	float getValueToSend();

	RealInputDataClass(int id, int pin, flt_call_back getValueCallBack);
	RealInputDataClass(int id, int pin, long overrideTxInterval, flt_call_back getValueCallBack);
};

#endif

