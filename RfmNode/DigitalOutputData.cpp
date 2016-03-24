// author: Danny Oleson - dannyoleson@gmail.com
// date: 12/28/2015
//
// for use with RfmNode.ino - https://github.com/dannyoleson/RFM69-MQTT-client/tree/master/RfmNode

#include "DigitalOutputData.h"

DigitalOutputDataClass::DigitalOutputDataClass(int id, uint8_t pin)
	: ComponentDataClass(id, pin)
{
	pinMode(pin, OUTPUT);
	state = 0;
	digitalWrite(pin, state);
}

int DigitalOutputDataClass::getState()
{
	return state;
}

void DigitalOutputDataClass::setState(int stateToSet)
{
	state = stateToSet;
	digitalWrite(m_dataPin, state);
}

int DigitalOutputDataClass::getValueToSend()
{
	return getState();
}