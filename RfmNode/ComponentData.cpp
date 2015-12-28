// 
// 
// 

#include "ComponentData.h"

ComponentDataClass::ComponentDataClass(int id, uint8_t pin)
{
	txInterval = 20 * 1000;
	deviceId = id;
	m_dataPin = pin;
	m_shouldSend = false;
	connectedComponents[connectedComponentsCount] = this;
	connectedComponentsCount++;
}

ComponentDataClass::ComponentDataClass(int id, uint8_t pin, long overrideTxInterval)
{
	txInterval = overrideTxInterval;
	deviceId = id;
	m_dataPin = pin;
	m_shouldSend = false;
	connectedComponents[connectedComponentsCount] = this;
	connectedComponentsCount++;
}

bool ComponentDataClass::getShouldSend()
{
	return m_shouldSend;
}

void ComponentDataClass::setShouldSend(bool shouldSendData)
{
	m_shouldSend = shouldSendData;
}

