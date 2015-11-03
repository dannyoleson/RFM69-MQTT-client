// 
// 
// 

#include "DigitalInputData.h"

DigitalInputDataClass::DigitalInputDataClass(int id, uint8_t pin, int delay)
	: ComponentDataClass(id, pin)
{
	inputDelay = delay;
	lastInputTime = 0;
	currentState = 0;
	lastState = currentState;
}

int DigitalInputDataClass::getState()
{
	lastState = currentState;
	currentState = digitalRead(dataPin);
	return currentState;
}

bool DigitalInputDataClass::getShouldSend()
{
	currentState = getState();
	if (((millis() - lastInputTime) < (long)inputDelay) && (lastState != currentState))
	{
		lastState = currentState;
	}
}

void DigitalInputDataClass::setShouldSend(bool shouldSendData)
{
	delay(5);
	lastInputTime = millis();		// take timestamp
	shouldSend = shouldSendData;
}

long DigitalInputDataClass::getLastInputTime()
{
	return lastInputTime;
}

int DigitalInputDataClass::getValueToSend()
{
	return getState();
}
