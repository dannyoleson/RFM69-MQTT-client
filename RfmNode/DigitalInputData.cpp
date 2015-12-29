// author: Danny Oleson - dannyoleson@gmail.com
// date: 12/28/2015
//
// for use with RfmNode.ino - https://github.com/dannyoleson/RFM69-MQTT-client/tree/master/RfmNode

#include "DigitalInputData.h"

DigitalInputDataClass::DigitalInputDataClass(int id, uint8_t pin, int inputDelay)
	: ComponentDataClass(id, pin)
{
	m_inputDelay = inputDelay;
	m_lastInputTime = -1;
	m_lastState = -1;
}

int DigitalInputDataClass::getState()
{
	return digitalRead(m_dataPin);
}

bool DigitalInputDataClass::getShouldSend()
{
	bool shouldSend = m_shouldSend;
	bool isFirstRun = m_lastInputTime == -1;

	int currentState = getState();
	if ((((currentTime - m_lastInputTime > (long)m_inputDelay) && m_lastState != currentState))
		|| isFirstRun)
	{
		m_lastState = currentState;
		shouldSend = true;
		m_lastInputTime = currentTime;
	}
	else if (periodicSendEnabled)
	{
		if ((currentTime - m_lastInputTime) > txInterval)
		{
			shouldSend = true;
			m_lastInputTime = currentTime;
		}
	}


	return shouldSend;
}

void DigitalInputDataClass::setShouldSend(bool shouldSendData)
{
	m_shouldSend = shouldSendData;
}

long DigitalInputDataClass::getLastInputTime()
{
	return m_lastInputTime;
}

int DigitalInputDataClass::getValueToSend()
{
	return getState();
}

bool DigitalInputDataClass::getIsButton()
{
	return m_isButton;
}

