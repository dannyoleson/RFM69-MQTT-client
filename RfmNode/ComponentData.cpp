// author: Danny Oleson - dannyoleson@gmail.com
// date: 12/28/2015
//
// for use with RfmNode.ino - https://github.com/dannyoleson/RFM69-MQTT-client/tree/master/RfmNode

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
		if (((millis() - m_lastSendTime) > txInterval))
		{
			m_shouldSend = true;
			m_lastSendTime = millis();
		}
	}
	return m_shouldSend;
}

void ComponentDataClass::setShouldSend(bool shouldSendData)
{
	m_shouldSend = shouldSendData;
}


