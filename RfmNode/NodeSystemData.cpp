// author: Danny Oleson - dannyoleson@gmail.com
// date: 12/28/2015
//
// for use with RfmNode.ino - https://github.com/dannyoleson/RFM69-MQTT-client/tree/master/RfmNode

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