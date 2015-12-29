// author: Danny Oleson - dannyoleson@gmail.com
// date: 12/28/2015
//
// for use with RfmNode.ino - https://github.com/dannyoleson/RFM69-MQTT-client/tree/master/RfmNode

#include "AnalogOutputData.h"

AnalogOutputDataClass::AnalogOutputDataClass(int id, uint8_t pin)
	: ComponentDataClass(id, pin)
{
	pinMode(pin, OUTPUT);
	state = 0;
	analogWrite(pin, state);
}

void AnalogOutputDataClass::setState(int stateToSet)
{
	state = stateToSet;
	analogWrite(m_dataPin, stateToSet);
}

int AnalogOutputDataClass::getValueToSend()
{
	return state;
}


