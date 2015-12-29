// RfmAioNodeLibrary.h
// author: Danny Oleson - dannyoleson@gmail.com
// date: 12/28/2015
//
// for use with RfmNode.ino - https://github.com/dannyoleson/RFM69-MQTT-client/tree/master/RfmNode

#ifndef _RFMAIONODELIBRARY_h
#define _RFMAIONODELIBRARY_h

#include <ButtonInputData.h>
#include <NodeSystemData.h>
#include <RealInputData.h>
#include <DigitalOutputData.h>
#include <DigitalInputData.h>
#include <AnalogOutputData.h>
#include <AnalogSensorData.h>
#include <ComponentData.h>

//AIO - All In One Node
#define VERSION "AIO V2.2"				// this value can be queried as device 3

#define WRITE 0
#define READ 1

#define GATEWAYID 1					// node ID of the Gateway is always 1

// device ID ranges
#define NODESYSTEMDEVICERANGEBOTTOM 0
#define NODESYSTEMDEVICERANGETOP 15
#define ISNODESYSTEMDEVICE(x) x >= NODESYSTEMDEVICERANGEBOTTOM && x <= NODESYSTEMDEVICERANGETOP

#define DIGITALOUTPUTRANGEBOTTOM 16
#define DIGITALOUTPUTRANGETOP 31
#define ISDIGITALOUTPUTDEVICE(x) x >= DIGITALOUTPUTRANGEBOTTOM && x <= DIGITALOUTPUTRANGETOP

#define ANALOGOUTPUTRANGEBOTTOM 32
#define ANALOGOUTPUTRANGETOP 39
#define ISANALOGOUTPUTDEVICE(x) x >= ANALOGOUTPUTRANGEBOTTOM && x <= ANALOGOUTPUTRANGETOP

#define DIGITALINPUTRANGEBOTTOM 40
#define DIGITALINPUTRANGETOP 47
#define ISDIGITALINPUTSENSOR(x) x >= DIGITALINPUTRANGEBOTTOM && x <= DIGITALINPUTRANGETOP

#define REALINPUTRANGEBOTTOM 48
#define REALINPUTRANGETOP 63
#define ISREALINPUTSENSOR(x) x >= REALINPUTRANGEBOTTOM && x <= REALINPUTRANGETOP

#define ANALOGSENSORRANGEBOTTOM 64
#define ANALOGSENSORRANGETOP 71
#define ISANALOGSENSOR(x) x >= ANALOGSENSORRANGEBOTTOM && x <= ANALOGSENSORRANGETOP

#define ERRORCODEBOTTOM 90
#define ERRORCODETOP 98
#define ISERRORCODE(x) x >= ERRORCODEBOTTOM && x <= ERRORCODETOP

// just one device - no need for top/bottom
#define ISWAKEUPCODE(x) x == 99

#define ACK_TIME 100					// max # of ms to wait for an ack

// defining NODESYSTEMDEVICES - not devices as such, but we treat them like devices to keep the code simple
// DO NOT EDIT THESE DEVICES
#define UPTIMEDEVICEID 0
#define TXINTERVALDEVICEID 1
#define RSSIDEVICEID 2
#define VERSIONDEVICEID 3
#define VOLTAGEDEVICEID 4
#define ACKDEVICEID 5
#define TOGGLEDEVICEID 6
#define TIMERDEVICEID 7
#define WIRELESSCONNECTIONERROR 90
#define UNSUPPORTEDDEVICE 92
#define WAKEUPNODE 99

#define SERIAL_BAUD 115200

typedef struct {					// Radio packet format
	int		nodeID;						// node identifier
	int		devID;						// device identifier 
	int		cmd;						// read or write
	long	intVal;						// integer payload
	float	fltVal;						// floating payload
	char	payLoad[32];					// string payload
} Message;

// these don't need new subclasses, but naming them differently keeps them easy to understand
typedef ComponentDataClass ErrorDataClass;
typedef ComponentDataClass WakeupSignalClass;

#endif

