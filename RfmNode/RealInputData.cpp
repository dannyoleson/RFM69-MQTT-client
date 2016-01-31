// author: Danny Oleson - dannyoleson@gmail.com
// date: 12/28/2015
//
// for use with RfmNode.ino - https://github.com/dannyoleson/RFM69-MQTT-client/tree/master/RfmNode

#include "RealInputData.h"

RealInputDataClass::RealInputDataClass(int id, int pin, flt_call_back getValueCallBack)
	: ComponentDataClass(id, pin)
{
	periodicSendEnabled = true;
	lastTxTime = -1;
	getValueCB = getValueCallBack;
}

RealInputDataClass::RealInputDataClass(int id, int pin, long overrideTxInterval, flt_call_back getValueCallBack)
	: ComponentDataClass(id, pin, overrideTxInterval)
{
	periodicSendEnabled = true;
	lastTxTime = -1;
	getValueCB = getValueCallBack;
}

bool RealInputDataClass::getShouldSend()
{
	bool firstRun = lastTxTime == -1;
	bool shouldSend = m_shouldSend;

	if (periodicSendEnabled)
	{
		if ((millis() - lastTxTime) > txInterval)
		{
			shouldSend = true;
		}
	}
	return (firstRun || shouldSend);
}

float RealInputDataClass::getValueToSend()
{
	if (isLowPowerNode)
	{
		int sensorHoldoffTime = 2500;
		int delayTime = sensorHoldoffTime - (millis() - thisCycleActualMillis);
		if (delayTime > 0)
		{
			delay(delayTime);
		}
	}

	lastTxTime = millis();
	return getValueCB();
}