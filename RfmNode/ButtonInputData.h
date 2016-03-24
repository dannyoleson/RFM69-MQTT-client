// ButtonInputData.h
// author: Danny Oleson - dannyoleson@gmail.com
// date: 12/28/2015
//
// for use with RfmNode.ino - https://github.com/dannyoleson/RFM69-MQTT-client/tree/master/RfmNode

#ifndef _BUTTONINPUTDATA_h
#define _BUTTONINPUTDATA_h

#include "DigitalInputData.h"
#include "DigitalOutputData.h"

class ButtonInputDataClass : public DigitalInputDataClass
{
protected:
	bool m_isToggle;
	int m_timerValue;
	bool m_timerRunning;
	long m_timerStartTime;
	DigitalOutputDataClass *m_outputDevice;

public:
	ButtonInputDataClass(int id, uint8_t pin, DigitalOutputDataClass *outputDevice);
	void setInputHandlerType(bool isToggle, int timerValue);
	void handleInput();
	void handleTimer();
};

#endif

