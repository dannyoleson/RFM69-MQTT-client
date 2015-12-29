// author: Danny Oleson - dannyoleson@gmail.com
// date: 12/28/2015
//
// for use with RfmNode.ino - https://github.com/dannyoleson/RFM69-MQTT-client/tree/master/RfmNode

#include "AnalogSensorData.h"

AnalogSensorDataClass::AnalogSensorDataClass(
	int sensorPollInterval, 
	int sensorDeltaThreshold, 
	uint8_t sensorDataPin, 
	int sensorDeviceId)
	: ComponentDataClass(sensorDeviceId, sensorDataPin)
{
	pollInterval = sensorPollInterval;
	deltaThreshold = sensorDeltaThreshold;
	lastPollTime = -1;
	lastReading = -1;
}

bool AnalogSensorDataClass::getShouldSend()
{
	bool shouldSend = m_shouldSend;
	bool isFirstRun = lastPollTime == -1;

	if (((currentTime - lastPollTime) > pollInterval) || isFirstRun)
	{
		int currentLevel = analogRead(m_dataPin);
		lastPollTime = currentTime;

		if (currentLevel < lastReading - deltaThreshold ||
			currentLevel > lastReading + deltaThreshold)
		{
			lastReading = currentLevel;
			valueToSend = currentLevel;
			shouldSend = true;
		}
	}
	else if (periodicSendEnabled)
	{
		if ((currentTime - lastPollTime) > txInterval)
		{
			lastPollTime = currentTime;
			shouldSend = true;
		}
	}

	return shouldSend;
}

int AnalogSensorDataClass::getValueToSend()
{
	return valueToSend;
}

