// AnalogSensorData.h

#ifndef _ANALOGSENSORDATA_h
#define _ANALOGSENSORDATA_h

#include "ComponentData.h"

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class AnalogSensorDataClass : public ComponentDataClass
{
 protected:

 public:
	int		pollInterval;
	long	lastPollTime;
	int		lastReading;
	int		deltaThreshold; // +/- this delta will trigger a transmission of data
	int		valueToSend;

	AnalogSensorDataClass(int sensorPollInterval, int sensorDeltaThreshold, uint8_t sensorDataPin, int sensorDeviceId);
	bool getShouldSend();
	int getValueToSend();
};

#endif

