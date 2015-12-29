// ButtonInputData.h

#ifndef _BUTTONINPUTDATA_h
#define _BUTTONINPUTDATA_h

#include "DigitalInputData.h"
#include "DigitalOutputData.h"

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

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

