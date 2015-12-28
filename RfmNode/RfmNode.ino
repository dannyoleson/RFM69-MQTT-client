// RFM69 node sketch
//
// This node talks to the MQTT-Gateway and will:
// - send sensor data periodically and on-demand
// - receive commands from the gateway to control actuators
// - receive commands from the gateway to change settings
//
// Several nodes can operate within a single network; each having a unique node ID.
// On startup the node operates with default values, set on compilation.
//
// Hardware used is a 3.3 Volt 8MHz arduino Pro; this is easier to interface to RFM69 
//
// A DHT-11 is used for temperature & humidity measurements, other sensors and outputs can be added easily.
//
// Message format is: nodeID/deviceID/command/integer/float/string
//
// Depending on the type of data (integer, float or string) one of the payload variables is used
// Command = 0 means write a value in the node, cmd = 1 means read a value 
//
// Current defined devices are:
//
//	0	uptime:			read uptime node in minutes
//	1	node:			read/set transmission interval in seconds, 0 means no periodic transmission
//	2	RSSI:			read radio signal strength
//	3	Version:		read version node software
//	4	voltage:		read battery level
//	5	ACK:			read/set acknowledge message after a 'set' request
//	6	toggle:			read/set toggle function on button press
//	7	timer:			read/set activation timer after button press in seconds, 0 means no timer
//	
//	16	actuator:		read/set LED or relay output
//	40	Button:			tx only: message sent when button pressed
//	48	temperature:		read temperature
//	49	humidity:		read humidity
//  64  light:			read light level
//  65  flame:			read flame sensor
//	66  gas:			read gas sensor
//	90	error:			tx only: error message if no wireless connection (generated by gateway)
//	92	error:			tx only: device not supported
//	99	wakeup:			tx only: first message sent on node startup
//
// 	The button can be set to:
//	- generate a message on each press (limited to one per 10 seconds) and/or
//	- toggle the output ACT1 (local node function) or
//	- activate the output for a fixed time period (local node function)
//
//	A debug mode is included which outputs messages on the serial output
//
//	RFM69 Library by Felix Rusu - felix@lowpowerlab.com
//	Get the RFM69 library at: https://github.com/LowPowerLab/
//
//	version 1.7 by Computourist@gmail.com december 2014
//	version 2.0 increased payload size; implemented node uptime; standard device type convention; error handling .
//	version 2.1 removed device 8; changed handling of device 40; compatible with gateway V2.2	; march 2015
//  version 2.2 lewishollow - dannyoleson@gmail.com - changed format to be object-oriented.  This should make adding devices much easier.

#include "ButtonInputData.h"
#include "NodeSystemData.h"
#include "RealInputData.h"
#include "DigitalOutputData.h"
#include "DigitalInputData.h"
#include "AnalogOutputData.h"
#include "AnalogSensorData.h"
#include "ComponentData.h"
#include <RFM69.h>
#include <RFM69_ATC.h>
#include <SPI.h>
#include <DHT.h>
#include <LowPower.h>

//
// CONFIGURATION PARAMETERS
//
#define NODEID 3 					// unique node ID within the closed network
#define GATEWAYID 1					// node ID of the Gateway is always 1
#define NETWORKID 100					// network ID of the network
#define ENCRYPTKEY "5029386215036408" 			// 16-char encryption key; same as on Gateway!
#define DEBUG						// uncomment for debugging
#define VERSION "NDE V2.2"				// this value can be queried as device 3
#define LOWPOWERNODE				// uncomment for battery-powered node

#define WRITE 0
#define READ 1

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

// Wireless settings	Match frequency to the hardware version of the radio
#define FREQUENCY RF69_433MHZ
//#define FREQUENCY RF69_868MHZ
//#define FREQUENCY RF69_915MHZ

#define IS_RFM69HW 					// uncomment only for RFM69HW! 
#define ACK_TIME 100					// max # of ms to wait for an ack



// enable or disable common devices here
//#define PHOTOSENSORENABLED
#define REEDSWITCHENABLED
//#define BUTTONENABLED
//#define ACTUATORENABLED
//#define FLAMESENSORENABLED
//#define GASSENSORENABLED
#define DHTSENSORENABLED

// Binary input settings
#ifdef BUTTONENABLED
#define BTN 8						// Button pin
#define BTNDEVICEID 40
#endif //BUTTONENABLED

#ifdef REEDSWITCHENABLED
#define REEDSWITCHPIN 3
#define REEDSWITCHDEVICEID 41
#endif //REEDSWITCHENABLED

// Binary output settings
#ifdef ACTUATORENABLED
#define ACT1 9						// Actuator pin (LED or relay)
#define ACTUATORDEVICEID 16
#endif //ACTUATORENABLED

// Analog sensor settings
#ifdef PHOTOSENSORENABLED
#define LIGHTPIN A0
#define LIGHTSENSORDEVICEID 64
#endif //PHOTOSENSORENABLED

#ifdef FLAMESENSORENABLED
#define FLAMEPIN A1
#define FLAMESENSORDEVICEID 65
#endif //FLAMESENSORENABLED

#ifdef GASSENSORENABLED
#define GASPIN A2
#define GASSENSORDEVICEID 66
#endif //GASSENSORENABLED

// DHT sensor setting
#ifdef DHTSENSORENABLED
#define DHTPIN 4					// DHT data connection
#define DHTTEMPDEVICEID 48
#define DHTHUMIDITYDEVICEID 49
#define	DHTTYPE	DHT22					// type of sensor
#endif //DHTSENSORENABLED


// defining NODESYSTEMDEVICES - not devices as such, but we treat them like devices to keep the code simple
// DO NOT EDIT THESE DEVICES
#define UPTIMEDEVICEID 0
#define TXINTERVALDEVICEID 1
#define RSSIDEVICEID 2
#define VERSIONDEVICEID 3
#define VOLTAGEDEVICEID 4
#define ACKDEVICEID 5
#define WIRELESSCONNECTIONERROR 90
#define UNSUPPORTEDDEVICE 92
#define WAKEUPNODE 99

#define SERIAL_BAUD 115200

//
//	VARIABLES
//
bool	setAck = false;					// send ACK message on 'SET' request
bool	promiscuousMode = false; 			// only listen to nodes within the closed network

typedef struct {					// Radio packet format
	int		nodeID;						// node identifier
	int		devID;						// device identifier 
	int		cmd;						// read or write
	long	intVal;						// integer payload
	float	fltVal;						// floating payload
	char	payLoad[32];					// string payload
} Message;

Message mes;

RFM69_ATC radio;

// adding components in setup() will add to this array and increment the count
ComponentDataClass *connectedComponents[100];
int connectedComponentsCount = 0;

// these don't need new subclasses, but naming them differently keeps them easy to understand
typedef ComponentDataClass ErrorDataClass;
typedef ComponentDataClass WakeupSignalClass;

// declare node system devices, error codes and wake up signal
// DO NOT MODIFY
NodeSystemDataClass *uptimeData;
NodeSystemDataClass *txIntervalData;
NodeSystemDataClass *rssiData;
NodeSystemDataClass *versionData;
NodeSystemDataClass *voltageData;
NodeSystemDataClass *ackData;
NodeSystemDataClass *toggleData;
NodeSystemDataClass *timerData;

ErrorDataClass *connectionError;
ErrorDataClass *unknownDevice;

WakeupSignalClass *wakeUp;

// To add devices, declare them below in the appropriate category

// declare binary output devices (LED, relay): 16-31
#ifdef ACTUATORENABLED
DigitalOutputDataClass *actuatorData;
#endif //ACTUATORENABLED

// declare analog output devices (pwm, dimmer): 32-39

// declare binary input devices (button, switch, PIR-sensor): 40-47
#ifdef BUTTONENABLED
#define TOGGLEDEVICEID 6
#define TIMERDEVICEID 7
long	timeInterval = 20 * 1000;				// timer interval in ms
bool	toggleOnButton = true;				// toggle output on button press
DigitalInputDataClass *buttonData;
#endif //BUTTONENABLED

#ifdef REEDSWITCHENABLED
DigitalInputDataClass *reedSwitchData;
#endif //REEDSWITCHENABLED

#ifdef DHTSENSORENABLED
DHT dht(DHTPIN, DHTTYPE, 3);			// initialise temp/humidity sensor for 3.3 Volt arduino
//DHT dht(DHTPIN, DHTTYPE);				// 5 volt power
// declare real value sensors (temperature, humidity): 48-63
RealInputDataClass *dhtTempSensorData;
RealInputDataClass *dhtHumSensorData;
#endif //DHTSENSORENABLED

// declare analog input sensors used in this node (temp, humidity): 64-71
#ifdef PHOTOSENSORENABLED
AnalogSensorDataClass *lightSensorData;
#endif //PHOTOSENSORENABLED

#ifdef FLAMESENSORENABLED
AnalogSensorDataClass *flameSensorData;
#endif //FLAMESENSORENABLED

#ifdef GASSENSORENABLED
AnalogSensorDataClass *gasSensorData;
#endif //GASSENSORENABELD

void wakeUpHandler()
{
	// wakeup handler
}

//use this to track how many times we've woken up
long numWakes;
unsigned long currentTime;
long thisCycleActualMillis;

//
//=====================		SETUP	========================================
//
void setup() {
	// instantiate node system devices - DO NOT MODIFY
	uptimeData = new NodeSystemDataClass(UPTIMEDEVICEID, NULL, &getUptime);
	txIntervalData = new NodeSystemDataClass(TXINTERVALDEVICEID, NULL, &getTxInterval);
	rssiData = new NodeSystemDataClass(RSSIDEVICEID, NULL, &getSignalStrength);
	versionData = new NodeSystemDataClass(VERSIONDEVICEID, NULL, NULL);
	voltageData = new NodeSystemDataClass(VOLTAGEDEVICEID, NULL, &getVoltage);
	ackData = new NodeSystemDataClass(ACKDEVICEID, NULL, &getAck);
#ifdef LOWPOWERNODE
	//pinMode(REEDSWITCHPIN, INPUT);
	// if running on low power, you won't be able to query.  These will always send their status ever txInterval seconds
	uptimeData->periodicSendEnabled = true;
	rssiData->periodicSendEnabled = true;
	voltageData->periodicSendEnabled = true;
#endif //LOWPOWERNODE

#ifdef BUTTONENABLED
	toggleData = new NodeSystemDataClass(TOGGLEDEVICEID, NULL, &getToggleState);
	timerData = new NodeSystemDataClass(TIMERDEVICEID, NULL, &getTimerInterval);
#endif //BUTTONENABLED

	// instantiate error devices
	connectionError = new ErrorDataClass(WIRELESSCONNECTIONERROR, NULL);
	unknownDevice = new ErrorDataClass(UNSUPPORTEDDEVICE, NULL);

	// instantiate our one wakeup device - DO NOT MODIFY
	wakeUp = new WakeupSignalClass(WAKEUPNODE, NULL);
	wakeUp->setShouldSend(true); //always send wakeup on first loop

	// instantiate binary output devices - DO NOT MODIFY
#ifdef ACTUATORENABLED
	actuatorData = new DigitalOutputDataClass(ACTUATORDEVICEID, ACT1);
#endif //ACTUATORENABLED

	// instantiate binary input devices - DO NOT MODIFY
#ifdef BUTTONENABLED
	int buttonDelay = 2 * 1000;
	buttonData = new DigitalInputDataClass(BTNDEVICEID, BTN, buttonDelay);
#endif //BUTTONENABLED

#ifdef REEDSWITCHENABLED
	reedSwitchData = new DigitalInputDataClass(REEDSWITCHDEVICEID, REEDSWITCHPIN, 1000);
	//reedSwitchData->periodicSendEnabled = true;
#endif //REEDSWITCHENABLED

	// instantiate real data sensors - callbacks are required
#ifdef DHTSENSORENABLED	
	dht.begin();
	bool useFarenheit = true;
	bool forceReading = false;
	long overrideTxInterval = 30 * 1000;
	dhtTempSensorData = new RealInputDataClass(DHTTEMPDEVICEID, DHTPIN, overrideTxInterval, &getDhtTemperatureFarenheit);
	dhtTempSensorData->periodicSendEnabled = true;
	dhtHumSensorData = new RealInputDataClass(DHTHUMIDITYDEVICEID, DHTPIN, overrideTxInterval, &getDhtHumidity);
	dhtHumSensorData->periodicSendEnabled = true;
#endif //DHTSENSORENABLED

	// instantiate analog sensors - no callback required
#ifdef PHOTOSENSORENABLED
	int lightDeltaThreshold = 50;
	lightSensorData = new AnalogSensorDataClass(2 * 1000, lightDeltaThreshold, LIGHTPIN, LIGHTSENSORDEVICEID);
#endif //PHOTOSENSORENABLED

#ifdef FLAMESENSORENABLED
	int flameDeltaThreshold = 20;
	flameSensorData = new AnalogSensorDataClass(2 * 1000, flameDeltaThreshold, FLAMEPIN, FLAMESENSORDEVICEID);
#endif //FLAMESENSORENABLED
	
#ifdef GASSENSORENABLED
	int gasDeltaThreshold = 70;
	gasSensorData = new AnalogSensorDataClass(5 * 1000, gasDeltaThreshold, GASPIN, GASSENSORDEVICEID);
#endif //GASSENSORENABLED

#ifdef DEBUG
	Serial.begin(SERIAL_BAUD);
#endif
	radio.initialize(FREQUENCY, NODEID, NETWORKID);		// initialise radio 
#ifdef IS_RFM69HW
	radio.setHighPower(); 					// only for RFM69HW!
#endif
	radio.encrypt(ENCRYPTKEY);				// set radio encryption	`
	radio.promiscuous(promiscuousMode);			// only listen to closed network

	radio.enableAutoPower(-90);

#ifdef DEBUG
	Serial.print("Node Software Version ");
	Serial.println(VERSION);
	Serial.print("\nTransmitting at ");
	Serial.print(FREQUENCY == RF69_433MHZ ? 433 : FREQUENCY == RF69_868MHZ ? 868 : 915);
	Serial.println(" Mhz...");
#endif

	numWakes = 0;
}	// end setup

//
//
//====================		MAIN	========================================
//
void loop() {
	// RECEIVE radio input
#ifdef LOWPOWERNODE
	radio.sleep();
	//delay(500);
	//attachInterrupt(1, wakeUpHandler, HIGH);
	//attachInterrupt(1, wakeUpHandler, LOW);
	LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
	thisCycleActualMillis = millis();
	numWakes++;
	currentTime =  millis() + (numWakes * 2000);
	Serial.println(currentTime);
	//detachInterrupt(1);
	delay(500);
#else
	if (receiveData())
	{
		parseCmd();				// receive and parse any radio input
	}
	currentTime = millis();
#endif

	// find buttons and handle their timers appropriately
	for (int componentNum = 0; componentNum < connectedComponentsCount; componentNum++)
	{
		if (ISDIGITALINPUTSENSOR(componentNum))
		{
			DigitalInputDataClass *thisDigitalInput = (DigitalInputDataClass *)connectedComponents[componentNum];
			if (thisDigitalInput->getIsButton())
			{
				thisDigitalInput->handleInput();
				thisDigitalInput->handleTimer();
			}
		}
	}

	// SEND RADIO PACKETS
	sendMsg();
}		// end loop

//
//
//=====================		FUNCTIONS	==========================================

//
//========		RECEIVEDATA : receive data from gateway over radio
//

bool receiveData() 
{
	bool validPacket = false;
	if (radio.receiveDone()) // check for received packets
	{			
		validPacket = radio.DATALEN == sizeof(mes);
		if (validPacket)
		{
			mes = *(Message*)radio.DATA;
			validPacket = true;				// YES, we have a packet !
		}

#ifdef DEBUG
		if (!validPacket)
		{
			Serial.println("invalid message structure..");
		}
		else
		{
			Serial.print(mes.devID);
			Serial.print(", ");
			Serial.print(mes.cmd);
			Serial.print(", ");
			Serial.print(mes.intVal);
			Serial.print(", ");
			Serial.print(mes.fltVal);
			Serial.print(", RSSI= ");
			Serial.println(radio.RSSI);
			Serial.print("Node: ");
			Serial.println(mes.nodeID);
		}
#endif
	}

	if (radio.ACKRequested())
	{
		radio.sendACK();		// respond to any ACK request
	}

	return validPacket;					// return code indicates packet received
}		// end recieveData

//
//
//==============		PARSECMD: analyse messages and execute commands received from gateway
//

void parseCmd() {					// parse messages received from the gateway
	bool deviceFound = false;
	for (int connectedComponentNumber = 0; connectedComponentNumber < connectedComponentsCount; connectedComponentNumber++)
	{
		ComponentDataClass *thisDevice = connectedComponents[connectedComponentNumber];
		thisDevice->setShouldSend(false);
		if (mes.devID == thisDevice->deviceId)
		{
			Serial.println(mes.devID);
			deviceFound = true;

			if (mes.cmd == READ)
			{
				thisDevice->setShouldSend(true);
			}

			if (ISNODESYSTEMDEVICE(mes.devID))
			{
				if (mes.devID == TXINTERVALDEVICEID)
				{
					if (mes.cmd == WRITE) // cmd == 0 means write a value
					{	
						thisDevice->setShouldSend(setAck);
						// change interval to radio packet value - min value is 10
						for (int deviceNum = 0; deviceNum < connectedComponentsCount; deviceNum++)
						{
							mes.intVal < 10 ? connectedComponents[deviceNum]->txInterval = 10
								: connectedComponents[deviceNum]->txInterval = mes.intVal;
						}

#ifdef DEBUG
						Serial.print("Setting interval to ");
						Serial.print(mes.intVal);
						Serial.println(" seconds");
#endif
					}
				}
				else if (mes.devID == ACKDEVICEID)
				{
					if (mes.cmd == WRITE) 
					{
						// intVal will indicate whether or not to acknowledge
						// for safety, ensure it's 1 or 0
						setAck = !(!(mes.intVal));
						thisDevice->setShouldSend(setAck);			// acknowledge message ?
					}
				}
#ifdef BUTTONENABLED
				else if (mes.devID == TOGGLEDEVICEID)
				{
					if (mes.cmd == WRITE) 
					{
						toggleOnButton = !(!(mes.intVal));
						if (setAck)
						{
							thisDevice->setShouldSend(true);			// acknowledge message ?
						}
					}
				}
				else if (mes.devID == TIMERDEVICEID)
				{
					if (mes.cmd == WRITE) 
					{
						timeInterval = mes.intVal;			// change interval 
						if (timeInterval < 5 && timeInterval != 0)
						{
							timeInterval = 5;
						}
						thisDevice->setShouldSend(setAck);			// acknowledge message ?
					}							// cmd == 1 means read a value
				}
#endif //BUTTONENABLED
			}
#ifdef ACTUATORENABLED
			else if (ISDIGITALOUTPUTDEVICE(mes.devID))
			{
				if (mes.devID == ACTUATORDEVICEID)
				{
					if (mes.cmd == 0) // cmd == 0 means write 
					{					
						if (mes.intVal == 0 || mes.intVal == 1) 
						{
							actuatorData->setState(mes.intVal);
							actuatorData->setShouldSend(setAck);			// acknowledge message ?
						}
					}
				}
			}
#endif //ACTUATORENABELD
		}
	}
	if (!deviceFound)
	{
		unknownDevice->setShouldSend(true);
	}
}	// end parseCmd

//
//
//======================		SENDMSG: sends messages that are flagged for transmission
//

void sendMsg() {					// prepares values to be transmitted
	//bool tx = false; 					// transmission flag
	mes.nodeID = NODEID;
	mes.intVal = 0;
	mes.fltVal = 0;
	mes.cmd = 0;						// '0' means no action needed in gateway
	int i;
	for (i = 0; i < sizeof(VERSION); i++) {
		mes.payLoad[i] = VERSION[i];
	}
	mes.payLoad[i] = '\0';					// software version in payload string

	if (wakeUp->getShouldSend())
	{
		mes.devID = wakeUp->deviceId;
		txRadio();
		wakeUp->setShouldSend(false);
	}

	for (int componentNumber = 0; componentNumber < connectedComponentsCount; componentNumber++)
	{	
		if (ISNODESYSTEMDEVICE(connectedComponents[componentNumber]->deviceId))
		{
			NodeSystemDataClass *nodeDevice = (NodeSystemDataClass *)connectedComponents[componentNumber];
			if (nodeDevice->getShouldSend())
			{
				mes.devID = nodeDevice->deviceId;
				// node devices can be floats or ints - setting both to catch all
				mes.fltVal = nodeDevice->getValueToSend();
				mes.intVal = (int)nodeDevice->getValueToSend();
				txRadio();
				nodeDevice->setShouldSend(false);
			}
		}

		if (ISDIGITALOUTPUTDEVICE(connectedComponents[componentNumber]->deviceId))
		{
			DigitalOutputDataClass *digitalOutputDevice = (DigitalOutputDataClass *)connectedComponents[componentNumber];
			if (digitalOutputDevice->getShouldSend())
			{
				mes.devID = digitalOutputDevice->deviceId;
				mes.intVal = digitalOutputDevice->getValueToSend();
				txRadio();
				digitalOutputDevice->setShouldSend(false);
			}		
		}

		if (ISANALOGOUTPUTDEVICE(connectedComponents[componentNumber]->deviceId))
		{
			AnalogOutputDataClass *analogOutputDevice = (AnalogOutputDataClass *)connectedComponents[componentNumber];
			if (analogOutputDevice->getShouldSend())
			{
				mes.devID = analogOutputDevice->deviceId;
				mes.intVal = analogOutputDevice->getValueToSend();
				txRadio();
				analogOutputDevice->setShouldSend(false);
			}		
		}

		if (ISDIGITALINPUTSENSOR(connectedComponents[componentNumber]->deviceId))
		{
			DigitalInputDataClass *digitalInputDevice = (DigitalInputDataClass *)connectedComponents[componentNumber];
			if (digitalInputDevice->getShouldSend())
			{
				mes.devID = digitalInputDevice->deviceId;
				mes.intVal = digitalInputDevice->getValueToSend();
				txRadio();
				digitalInputDevice->setShouldSend(false);
			}
		}

		if (ISREALINPUTSENSOR(connectedComponents[componentNumber]->deviceId))
		{
			RealInputDataClass *realInputDevice = (RealInputDataClass *)connectedComponents[componentNumber];
			if (realInputDevice->getShouldSend())
			{
				mes.devID = realInputDevice->deviceId;
				mes.fltVal = realInputDevice->getValueToSend();
				txRadio();
				realInputDevice->setShouldSend(false);
			}
		}

		if (ISANALOGSENSOR(connectedComponents[componentNumber]->deviceId))
		{
			AnalogSensorDataClass *analogSensorDevice = (AnalogSensorDataClass *)connectedComponents[componentNumber];
			if (analogSensorDevice->getShouldSend())
			{
				mes.devID = analogSensorDevice->deviceId;
				mes.intVal = analogSensorDevice->getValueToSend();
				txRadio();
				analogSensorDevice->setShouldSend(false);
			}
		}

		if (ISERRORCODE(connectedComponents[componentNumber]->deviceId))
		{
			if (connectedComponents[componentNumber]->getShouldSend())
			{
				mes.intVal = mes.devID;
				mes.devID = connectedComponents[componentNumber]->deviceId;
				txRadio();
				connectedComponents[componentNumber]->setShouldSend(false);
			}
		}

		if (ISWAKEUPCODE(connectedComponents[componentNumber]->deviceId))
		{
			if (connectedComponents[componentNumber]->getShouldSend())
			{
				mes.devID = connectedComponents[componentNumber]->deviceId;
				txRadio();
				connectedComponents[componentNumber]->setShouldSend(false);
			}
		}
	}
}

//
//
//=======================		TXRADIO
//

void txRadio()						// Transmits the 'mes'-struct to the gateway
{
	if (radio.sendWithRetry(GATEWAYID, (const void*)(&mes), sizeof(mes), 3, ACK_TIME))
#ifdef DEBUG
	{
		Serial.print(" message ");
		Serial.print(mes.devID);
		Serial.println(" sent...");
	}
	else
	{
		Serial.println("No connection...");
	}
#endif
}	// end txRadio

//
//
//==============	functions to use for callbacks to pass to the device classes
//
// 

float getUptime()
{
	return currentTime / 60000;
}

float getTxInterval()
{
	int returnValue = -1;
	if (connectedComponentsCount > 0)
	{
		returnValue = connectedComponents[0]->txInterval;
	}
	return returnValue;
}

float getSignalStrength()
{
	return radio.RSSI;
}

float getVoltage()
{
	long voltage;					// Read 1.1V reference against AVcc
	ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
	delay(2);						// Wait for Vref to settle
	ADCSRA |= _BV(ADSC);			// Convert
	while (bit_is_set(ADCSRA, ADSC));
	voltage = ADCL;
	voltage |= ADCH << 8;
	voltage = 1126400L / voltage; 	// Back-calculate in mV
	return float(voltage / 1000.0);
}

float getAck()
{
	return setAck * 1.0;
}

#ifdef BUTTONENABLED
int getToggleState()
{
	return toggleOnButton;
}

int getTimerInterval()
{
	return timeInterval;
}
#endif //BUTTONENABLED

#ifdef DHTSENSORENABLED
float getDhtTemperatureFarenheit()
{
	return dht.readTemperature(true);
}

float getDhtHumidity()
{
	return dht.readHumidity();
}
#endif //DHTSENSORENABLED