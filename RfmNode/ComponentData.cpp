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
	periodicSendEnabled = false;
	m_lastSendTime = -1;
	connectedComponents[connectedComponentsCount] = this;
	connectedComponentsCount++;
}

ComponentDataClass::ComponentDataClass(int id, uint8_t pin, long overrideTxInterval)
{
	txInterval = overrideTxInterval;
	deviceId = id;
	m_dataPin = pin;
	m_shouldSend = false;
	periodicSendEnabled = false;
	m_lastSendTime -1;
	connectedComponents[connectedComponentsCount] = this;
	connectedComponentsCount++;
}

bool ComponentDataClass::getShouldSend()
{
	if (periodicSendEnabled)
	{
		if (((currentTime - m_lastSendTime) > txInterval))
		{
			m_shouldSend = true;
			m_lastSendTime = currentTime;
		}
	}
	return m_shouldSend;
}

void ComponentDataClass::setShouldSend(bool shouldSendData)
{
	m_shouldSend = shouldSendData;
}


