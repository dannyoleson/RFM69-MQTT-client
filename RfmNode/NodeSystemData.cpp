// 
// 
// 

#include "NodeSystemData.h"

NodeSystemDataClass::NodeSystemDataClass(int id, uint8_t pin, flt_call_back getValueCallBack)
	: ComponentDataClass(id, pin)
{
	getValueCB = getValueCallBack;
}

float NodeSystemDataClass::getValueToSend()
{
	return getValueCB();
}
