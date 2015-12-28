// 
// 
// 

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