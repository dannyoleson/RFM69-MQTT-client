// AnalogSensorData.h
// author: Danny Oleson - dannyoleson@gmail.com
// date: 12/28/2015
//
// for use with RfmNode.ino - https://github.com/dannyoleson/RFM69-MQTT-client/tree/master/RfmNode

#ifndef _ANALOGSENSORDATA_h
#define _ANALOGSENSORDATA_h

#include "ComponentData.h"

class AnalogSensorDataClass : public ComponentDataClass
{
 protected:
	 int	m_pollInterval;
	 long	m_lastPollTime;
	 long	m_lastSendTime;
	 int	m_currentReading;
	 int	m_deltaThreshold; // +/- this delta will trigger a transmission of data
	 int	m_lastSentValue;
	 void	readSensorValue();

public:
	AnalogSensorDataClass(int sensorPollInterval, int sensorDeltaThreshold, uint8_t sensorDataPin, int sensorDeviceId);
	bool getShouldSend();
	int getValueToSend();
};

#endif

