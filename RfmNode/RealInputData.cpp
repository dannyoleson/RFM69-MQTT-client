// author: Danny Oleson - dannyoleson@gmail.com
// date: 12/28/2015
//
// for use with RfmNode.ino - https://github.com/dannyoleson/RFM69-MQTT-client/tree/master/RfmNode

#include "RealInputData.h"

RealInputDataClass::RealInputDataClass(int id, int pin, flt_call_back getValueCallBack)
	: ComponentDataClass(id, pin)
{
	periodicSendEnabled = true;
	Serial.println(id);
	lastTxTime = -1;
	getValueCB = getValueCallBack;
}

RealInputDataClass::RealInputDataClass(int id, int pin, long overrideTxInterval, flt_call_back getValueCallBack)
	: ComponentDataClass(id, pin, overrideTxInterval)
{
	periodicSendEnabled = true;
	Serial.println(id);
	lastTxTime = -1;
	getValueCB = getValueCallBack;
}

bool RealInputDataClass::getShouldSend()
{
	bool firstRun = lastTxTime == -1;

	if (periodicSendEnabled)
	{
		m_shouldSend = (currentTime - lastTxTime) > txInterval;
	}
	return (firstRun || m_shouldSend);
}

float RealInputDataClass::getValueToSend()
{
#ifdef LOWPOWERNODE
	int sensorHoldoffTime = 2500;
	int delayTime = sensorHoldoffTime - (millis() - thisCycleActualMillis);
	Serial.print("sensor has been powered on for ");
	Serial.println(millis() - thisCycleActualMillis);
	Serial.print("waiting for - ");
	Serial.println(delayTime);
	if (delayTime > 0)
	{
		delay(delayTime);
	}
#endif //LOWPOWERNODE

	lastTxTime = currentTime;
	return getValueCB();
}