// author: Danny Oleson - dannyoleson@gmail.com
// date: 12/28/2015
//
// for use with RfmNode.ino - https://github.com/dannyoleson/RFM69-MQTT-client/tree/master/RfmNode

#include "AnalogOutputData.h"

AnalogOutputDataClass::AnalogOutputDataClass(int id, uint8_t pin, float scale = 1.0)
	: ComponentDataClass(id, pin)
{
	pinMode(pin, OUTPUT);
	m_state = 0;
	m_scale = scale;
	analogWrite(pin, m_state);
}

void AnalogOutputDataClass::setState(int stateToSet)
{
	float fltState = (float)stateToSet*m_scale;
	m_state = (int)fltState;
	analogWrite(m_dataPin, m_state);
}

int AnalogOutputDataClass::getValueToSend()
{
	return m_state;
}

