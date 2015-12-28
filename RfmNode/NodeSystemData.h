// NodeSystemData.h

#ifndef _NODESYSTEMDATA_h
#define _NODESYSTEMDATA_h

#include "ComponentData.h"

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class NodeSystemDataClass : public ComponentDataClass
{
 protected:
	 flt_call_back getValueCB;

 public:
	 NodeSystemDataClass(int id, uint8_t pin, flt_call_back getValueCallBack);
	 float getValueToSend();
};

#endif

