// 
// 
// 

#include "RealInputData.h"

RealInputDataClass::RealInputDataClass(int id, int pin, flt_call_back getValueCallBack)
	: ComponentDataClass(id, pin)
{
	lastTxTime = -1;
	lastReading = -1;
	getValueCB = getValueCallBack;
}

bool RealInputDataClass::shouldSend()
{
	bool firstRun = lastTxTime == -1 || lastReading == -1;
	return (firstRun || ((millis() - lastTxTime) > txInterval));
}

float RealInputDataClass::getValueToSend()
{
	return getValueCB();
}