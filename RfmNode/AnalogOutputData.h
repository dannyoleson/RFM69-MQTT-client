// AnalogOutputData.h
// author: Danny Oleson - dannyoleson@gmail.com
// date: 12/28/2015
//
// for use with RfmNode.ino - https://github.com/dannyoleson/RFM69-MQTT-client/tree/master/RfmNode

#ifndef _ANALOGOUTPUTDATA_h
#define _ANALOGOUTPUTDATA_h

#include "ComponentData.h"

class AnalogOutputDataClass : public ComponentDataClass
{
protected:
	int	m_state;
	float m_scale;

public:
	AnalogOutputDataClass(int id, uint8_t pin, float scale);	
	void setState(int stateToSet);
	int getValueToSend();
};

#endif

