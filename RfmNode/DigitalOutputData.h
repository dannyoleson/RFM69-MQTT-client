// DigitalOutputData.h
// author: Danny Oleson - dannyoleson@gmail.com
// date: 12/28/2015
//
// for use with RfmNode.ino - https://github.com/dannyoleson/RFM69-MQTT-client/tree/master/RfmNode

#ifndef _DIGITALOUTPUTDATA_h
#define _DIGITALOUTPUTDATA_h

#include "ComponentData.h"

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

