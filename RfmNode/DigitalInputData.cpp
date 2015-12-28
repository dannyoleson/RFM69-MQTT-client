// 
// 
// 

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
	bool shouldSend = false;
	int currentState = getState();
	if (((millis() - m_lastInputTime > (long)m_inputDelay) && m_lastState != currentState))
	{
		m_lastState = currentState;
		shouldSend = true;
	}

	if (shouldSend || m_shouldSend)
	{
		m_lastInputTime = millis();
	}

	return shouldSend || m_shouldSend;
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

