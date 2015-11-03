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

bool AnalogSensorDataClass::shouldSend()
{
	bool firstRun = lastPollTime == -1 || lastReading == -1;
	if (firstRun || ((millis() - lastPollTime) > pollInterval))
	{
		int currentLevel = analogRead(dataPin);
		lastPollTime = millis();

		if (firstRun ||
			currentLevel < lastReading - deltaThreshold ||
			currentLevel > lastReading + deltaThreshold)
		{
			lastReading = currentLevel;
			valueToSend = currentLevel;
		}
	}
}

int AnalogSensorDataClass::getValueToSend()
{
	return analogRead(dataPin);
}

