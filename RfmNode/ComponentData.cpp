// 
// 
// 

#include "ComponentData.h"

ComponentDataClass::ComponentDataClass(int id, uint8_t pin)
{
	deviceId = id;
	dataPin = pin;
	shouldSend = false;
	connectedComponents[connectedComponentsCount] = this;
	connectedComponentsCount++;
}

bool ComponentDataClass::getShouldSend()
{
	return shouldSend;
}

void ComponentDataClass::setShouldSend(bool shouldSendData)
{
	shouldSendData = shouldSend;
}

