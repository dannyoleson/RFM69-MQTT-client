// 
// 
// 

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
	bool shouldSendData = false;
	if (((millis() - lastPollTime) > pollInterval))
	{
		if (m_shouldSend)
		{
			shouldSendData = true;
		}
		else
		{
			int currentLevel = analogRead(m_dataPin);
			lastPollTime = millis();

			if (currentLevel < lastReading - deltaThreshold ||
				currentLevel > lastReading + deltaThreshold)
			{
				lastReading = currentLevel;
				valueToSend = currentLevel;
				shouldSendData = true;
			}
		}
	}
	return shouldSendData;
}

int AnalogSensorDataClass::getValueToSend()
{
	return valueToSend;
}

