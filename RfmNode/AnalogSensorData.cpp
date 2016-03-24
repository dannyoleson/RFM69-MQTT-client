// author: Danny Oleson - dannyoleson@gmail.com
// date: 12/28/2015
//
// for use with RfmNode.ino - https://github.com/dannyoleson/RFM69-MQTT-client/tree/master/RfmNode

#include "AnalogSensorData.h"

AnalogSensorDataClass::AnalogSensorDataClass(
	int sensorDeviceId,
	uint8_t sensorDataPin,
	int sensorPollInterval, 
	int sensorDeltaThreshold)
	: ComponentDataClass(sensorDeviceId, sensorDataPin)
{
	m_pollInterval = sensorPollInterval;
	m_deltaThreshold = sensorDeltaThreshold;
	m_lastPollTime = -1;
	m_currentReading = -1;
	m_lastSendTime = -1;
}

bool AnalogSensorDataClass::getShouldSend()
{
	bool shouldSend = m_shouldSend;
	bool isFirstRun = m_lastSendTime == -1;

	readSensorValue();

	if (m_currentReading < (m_lastSentValue - m_deltaThreshold) ||
		m_currentReading > (m_lastSentValue + m_deltaThreshold))
	{
		shouldSend = true;
	}
	else if (periodicSendEnabled && (millis() - m_lastSendTime > txInterval)
		|| isFirstRun)
	{
		shouldSend = true;
	}

	return shouldSend;
}

void AnalogSensorDataClass::readSensorValue()
{
	bool isFirstRun = m_currentReading == -1;
	if ((millis() - m_lastPollTime) > m_pollInterval ||
		isFirstRun)
	{
		m_currentReading = analogRead(m_dataPin);
		m_lastPollTime = millis();
	}
}

int AnalogSensorDataClass::getValueToSend()
{
	m_lastSendTime = millis();
	m_lastSentValue = m_currentReading;
	return m_currentReading;
}

