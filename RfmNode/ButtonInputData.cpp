// 
// author: Danny Oleson - dannyoleson@gmail.com
// date: 12/28/2015
//
// for use with RfmNode.ino - https://github.com/dannyoleson/RFM69-MQTT-client/tree/master/RfmNode

#include "ButtonInputData.h"

ButtonInputDataClass::ButtonInputDataClass(int id, uint8_t pin, DigitalOutputDataClass *outputDevice)
	: DigitalInputDataClass(id, pin, 2 * 1000)
{
	m_isToggle = true;
	m_timerValue = 0;
	m_timerRunning = false;
	m_timerStartTime = -1;
	m_outputDevice = outputDevice;
	m_isButton = true;
}

// button can either be a toggle to turn an output on or off or it can be used with a timer
// to turn on a output for a specified period
// true for toggle, false for timer.  default is toggle
void ButtonInputDataClass::setInputHandlerType(bool isToggle, int timerValue = 0)
{
	m_isToggle = isToggle;
	m_timerValue = timerValue;
}

void ButtonInputDataClass::handleInput()
{
	if (((millis() - m_lastInputTime) > (long)m_inputDelay))
	{
		int currentState = getState();
		m_lastInputTime = millis();
		if (m_lastState != currentState)
		{
			m_shouldSend = true;
			m_lastState = currentState;
			if (currentState == LOW)
			{
				if (m_isToggle)
				{
					m_outputDevice->setState(!m_outputDevice->getState());
				}
				else if (m_timerValue > 0 && !m_timerRunning)
				{
					m_timerRunning = true;
					m_timerStartTime = millis();
					m_outputDevice->setState(HIGH);
				}
			}
		}
	}
}

void ButtonInputDataClass::handleTimer()
{
	if (m_timerRunning)
	{
		if (millis() - m_timerStartTime > m_timerValue)
		{
			m_timerRunning = false;
			m_outputDevice->setState(LOW);
		}
	}
}
