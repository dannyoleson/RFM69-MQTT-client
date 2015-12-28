// 
// 
// 

#include "RealInputData.h"

RealInputDataClass::RealInputDataClass(int id, int pin, flt_call_back getValueCallBack)
	: ComponentDataClass(id, pin)
{
	Serial.println(id);
	lastTxTime = -1;
	getValueCB = getValueCallBack;
}

RealInputDataClass::RealInputDataClass(int id, int pin, long overrideTxInterval, flt_call_back getValueCallBack)
	: ComponentDataClass(id, pin, overrideTxInterval)
{
	Serial.println(id);
	lastTxTime = -1;
	getValueCB = getValueCallBack;
}

bool RealInputDataClass::getShouldSend()
{

	bool firstRun = lastTxTime == -1;
	return (firstRun || m_shouldSend || ((millis() - lastTxTime) > txInterval));
}

float RealInputDataClass::getValueToSend()
{
	lastTxTime = millis();
	return getValueCB();
}