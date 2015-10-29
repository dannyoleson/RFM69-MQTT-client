// RFM69 DHT node sketch
//
// This node talks to the MQTT-Gateway and will:
// - send sensor data periodically and on-demand
// - receive commands from the gateway to control actuators
// - receive commands from the gateway to change settings
//
// Several nodes can operate within a single network; each have a unique node ID.
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

#include <RFM69.h>
#include <SPI.h>
#include <DHT.h>

//
// CONFIGURATION PARAMETERS
//
#define NODEID 2 					// unique node ID within the closed network
#define GATEWAYID 1					// node ID of the Gateway is always 1
#define NETWORKID 100					// network ID of the network
#define ENCRYPTKEY "xxxxxxxxxxxxxxxx" 			// 16-char encryption key; same as on Gateway!
#define DEBUG						// uncomment for debugging
#define VERSION "DHT V2.1"				// this value can be queried as device 3

// Wireless settings	Match frequency to the hardware version of the radio

#define FREQUENCY RF69_433MHZ
//#define FREQUENCY RF69_868MHZ
//#define FREQUENCY RF69_915MHZ

#define IS_RFM69HW 					// uncomment only for RFM69HW! 
#define ACK_TIME 100					// max # of ms to wait for an ack

// Light sensor setting
#define LIGHTPIN A0

// DHT 11 / sensor setting
#define DHTPIN 4					// DHT data connection
#define	DHTTYPE	DHT22					// type of sensor
#define ACT1 9						// Actuator pin (LED or relay)
#define BTN 8						// Button pin
#define SERIAL_BAUD 115200
#define HOLDOFF 2000					// blocking period between button messages

//
//	STARTUP DEFAULTS
//
long 	TXinterval = 20;				// periodic transmission interval in seconds
long	lastTxTime = -1;
long	TIMinterval = 20;				// timer interval in seconds
bool	ackButton = false;				// flag for message on button press
bool	toggleOnButton = true;				// toggle output on button press

											//
											//	VARIABLES
											//
long	lastPeriod = -1;				// timestamp last transmission
long 	lastBtnPress = -1;				// timestamp last buttonpress
long	lastMinute = -1;				// timestamp last minute
long	upTime = 0;					// uptime in minutes
float	hum, temp;					// humidity, temperature
int     lightLevel;                                     // photoresistor
int		ACT1State;					// status ACT1 output
int		signalStrength;					// radio signal strength
bool	setAck = false;					// send ACK message on 'SET' request
bool	send0, send1, send2, send3, send4;
bool	send5, send6, send7;
bool	send16, send40, send48, send49, send64, send92;		// message triggers
bool	promiscuousMode = false; 			// only listen to nodes within the closed network
bool	curState = true;				// current button state
bool	lastState = true;				// last button state
bool 	wakeUp = true;					// wakeup flag
bool	timerOnButton = false;				// timer output on button press
bool	msgBlock = false;				// flag to hold button messages to prevent overload



typedef struct {					// Radio packet format
	int	nodeID;						// node identifier
	int	devID;						// device identifier 
	int	cmd;						// read or write
	long	intVal;						// integer payload
	float	fltVal;						// floating payload
	char	payLoad[32];					// string payload
} Message;

Message mes;

//DHT dht(DHTPIN, DHTTYPE, 3);			// initialise temp/humidity sensor for 3.3 Volt arduino
DHT dht(DHTPIN, DHTTYPE);
RFM69 radio;

//photoresistor setup
int	lightPollInterval = 2 * 1000;		// poll interval for photoresistor
long lastLightPollTime = -1;
int currLightVal = -1;
int lastLightVal = -1;

//
//=====================		SETUP	========================================
//
void setup() {
#ifdef DEBUG
	Serial.begin(SERIAL_BAUD);
#endif
	pinMode(ACT1, OUTPUT);					// set actuator 1
	ACT1State = 0;
	digitalWrite(ACT1, ACT1State);
	dht.begin();						// initialise temp/hum sensor
	radio.initialize(FREQUENCY, NODEID, NETWORKID);		// initialise radio 
#ifdef IS_RFM69HW
	radio.setHighPower(); 					// only for RFM69HW!
#endif
	radio.encrypt(ENCRYPTKEY);				// set radio encryption	
	radio.promiscuous(promiscuousMode);			// only listen to closed network
	wakeUp = true;						// send wakeup message

#ifdef DEBUG
	Serial.print("Node Software Version ");
	Serial.println(VERSION);
	Serial.print("\nTransmitting at ");
	Serial.print(FREQUENCY == RF69_433MHZ ? 433 : FREQUENCY == RF69_868MHZ ? 868 : 915);
	Serial.println(" Mhz...");
#endif
}	// end setup

	//
	//
	//====================		MAIN	========================================
	//
void loop() {
	// RECEIVE radio input
	//
	if (receiveData())
	{
		parseCmd();				// receive and parse any radio input
	}
	// DETECT INPUT CHANGE
	//
	curState = digitalRead(BTN);
	msgBlock = ((millis() - lastBtnPress) < HOLDOFF);		// hold-off time for additional button messages
	if (!msgBlock && (curState != lastState)) {			// input changed ?
		delay(5);
		lastBtnPress = millis();							// take timestamp
		send40 = true;										// set button message flag
		if (curState == LOW) {
			if (toggleOnButton) {								// button in toggle state ?
				ACT1State = !ACT1State; 							// toggle output
				digitalWrite(ACT1, ACT1State);
			}
			else if (TIMinterval > 0 && !timerOnButton) {			// button in timer state ?
				timerOnButton = true;								// start timer interval
				ACT1State = HIGH;									// switch on ACT1
				digitalWrite(ACT1, ACT1State);
			}
		}
		lastState = curState;
	}

	// TIMER CHECK
	//

	if (TIMinterval > 0 && timerOnButton)			// =0 means no timer
	{
		if (millis() - lastBtnPress > TIMinterval * 1000) {	// timer expired ?
			timerOnButton = false;				// then end timer interval 
			ACT1State = LOW;				// and switch off Actuator
			digitalWrite(ACT1, ACT1State);
		}
	}

	// UPTIME 
	//

	if (lastMinute != (millis() / 60000)) {			// another minute passed ?
		lastMinute = millis() / 60000;
		upTime++;
	}

	// PERIODIC TRANSMISSION
	//

	if (TXinterval > 0) {
		int currPeriod = millis() / (TXinterval * 1000);
		if (currPeriod != lastPeriod) {				// interval elapsed ?
			lastPeriod = currPeriod;

			// list of sensordata to be sent periodically..
			// remove comment to include parameter in transmission

			//send1 = true;					// send transmission interval
			//send2 = true; 					// signal strength
			//send4 = true;					// voltage level
			//send16 = true;					// actuator state
			send48 = true;					// send temperature
			send49 = true;					// send humidity
		}
	}

	// POLL LIGHT SENSOR FOR LARGE DELTAS
	//
	bool firstRun = lastLightPollTime == -1 || lastLightVal == -1;
	if (firstRun || ((millis() - lastLightPollTime) > lightPollInterval))
	{
		currLightVal = analogRead(LIGHTPIN);
		lastLightPollTime = millis();

		if (firstRun || currLightVal < lastLightVal - 50 || currLightVal > lastLightVal + 100)
		{
			lastLightVal = currLightVal;
			send64 = true;
		}
	}


	// SEND RADIO PACKETS
	//

	sendMsg();						// send any radio messages 

}		// end loop

		//
		//
		//=====================		FUNCTIONS	==========================================

		//
		//========		RECEIVEDATA : receive data from gateway over radio
		//

bool receiveData() {
	bool validPacket = false;
	if (radio.receiveDone()) {			// check for received packets
		if (radio.DATALEN != sizeof(mes))			// wrong message size means trouble
#ifdef DEBUG
			Serial.println("invalid message structure..")
#endif
			;
		else
		{
			mes = *(Message*)radio.DATA;
			validPacket = true;				// YES, we have a packet !
			signalStrength = radio.RSSI;
#ifdef DEBUG
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
#endif	
		}
	}
	if (radio.ACKRequested()) radio.sendACK();		// respond to any ACK request
	return validPacket;					// return code indicates packet received
}		// end recieveData

		//
		//
		//==============		PARSECMD: analyse messages and execute commands received from gateway
		//

void parseCmd() {					// parse messages received from the gateway
	send0 = false;						// initialise all send triggers
	send1 = false;
	send2 = false;
	send3 = false;
	send4 = false;
	send5 = false;
	send6 = false;
	send7 = false;
	send16 = false;
	send40 = false;
	send48 = false;
	send49 = false;
	send64 = false;
	send92 = false;

	switch (mes.devID)					// devID indicates device (sensor) type
	{
	case (0) :						// uptime
		if (mes.cmd == 1) send0 = true;
		break;
	case (1) :						// polling interval in seconds
		if (mes.cmd == 0) {					// cmd == 0 means write a value
			TXinterval = mes.intVal;			// change interval to radio packet value
			if (TXinterval <10 && TXinterval != 0) TXinterval = 10;	// minimum interval is 10 seconds
			if (setAck) send1 = true;			// send message if required
#ifdef DEBUG
			Serial.print("Setting interval to ");
			Serial.print(TXinterval);
			Serial.println(" seconds");
#endif
		}
		else send1 = true;					// cmd == 1 is a read request, so send polling interval 
		break;
	case (2) : 						// signal strength
		if (mes.cmd == 1) send2 = true;
		break;
	case (3) : 						// software version
		if (mes.cmd == 1) send3 = true;
		break;
	case (4) : 						// battery level
		if (mes.cmd == 1) send4 = true;
		break;
	case (5) : 						// set ack status
		if (mes.cmd == 0) {
			if (mes.intVal == 0) setAck = false;
			if (mes.intVal == 1) setAck = true;
			if (setAck) send5 = true;			// acknowledge message ?
		}
		else send5 = true;				// read request means schedule a message
		break;
	case (6) : 						// set toggle
		if (mes.cmd == 0) {
			if (mes.intVal == 0) toggleOnButton = false;
			if (mes.intVal == 1) toggleOnButton = true;
			if (setAck) send6 = true;			// acknowledge message ?
		}
		else send6 = true;
		break;
	case (7) :						// timer interval in seconds
		if (mes.cmd == 0) {					// cmd == 0 means write a value
			TIMinterval = mes.intVal;			// change interval 
			if (TIMinterval <5 && TIMinterval != 0) TIMinterval = 5;
			if (setAck) send7 = true;			// acknowledge message ?
		}							// cmd == 1 means read a value
		else send7 = true;					// send timing interval 
		break;
	case (16) :						// Actuator 1
		if (mes.cmd == 0) {					// cmd == 0 means write
			if (mes.intVal == 0 || mes.intVal == 1) {
				ACT1State = mes.intVal;
				digitalWrite(ACT1, ACT1State);
				if (setAck) send16 = true;			// acknowledge message ?
#ifdef DEBUG	
				Serial.print("Set LED to ");
				Serial.println(ACT1State);
#endif
			}
		}
		else send16 = true;					// cmd == 1 means read
		break;
	case (40) :						// binary input
		if (mes.cmd == 1) send40 = true;
		break;
	case (48) :						// temperature
		if (mes.cmd == 1) send48 = true;
		break;
	case (49) :						// humidity
		if (mes.cmd == 1) send49 = true;
		break;
	case (64) :
		if (mes.cmd == 1) send64 = true;
		break;
	default: send92 = true;					// no valid device parsed
	}
}	// end parseCmd

	//
	//
	//======================		SENDMSG: sends messages that are flagged for transmission
	//

void sendMsg() {					// prepares values to be transmitted
	bool tx = false; 					// transmission flag
	mes.nodeID = NODEID;
	mes.intVal = 0;
	mes.fltVal = 0;
	mes.cmd = 0;						// '0' means no action needed in gateway
	int i;
	for (i = 0; i < sizeof(VERSION); i++) {
		mes.payLoad[i] = VERSION[i];
	}
	mes.payLoad[i] = '\0';					// software version in payload string

	if (wakeUp) {						// send wakeUp call 
		mes.devID = 99;
		wakeUp = false;					// reset transmission flag for this message
		txRadio();					// transmit
	}
	if (send0) {
		mes.devID = 0;
		mes.intVal = upTime;				// minutes uptime
		send0 = false;
		txRadio();
	}
	if (send1) {						// transmission interval
		mes.devID = 1;
		mes.intVal = TXinterval;			// seconds (integer)
		send1 = false;
		txRadio();
	}
	if (send2) {
		mes.devID = 2;
		mes.intVal = signalStrength;			// signal strength (integer)
		send2 = false;
		txRadio();
	}
	if (send3) {						// node software version (string)
		mes.devID = 3;					// already stored in payload string
		send3 = false;
		txRadio();
	}
	if (send4) {						// measure voltage..
		mes.devID = 4;
		long result;					// Read 1.1V reference against AVcc
		ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
		delay(2);					// Wait for Vref to settle
		ADCSRA |= _BV(ADSC);				// Convert
		while (bit_is_set(ADCSRA, ADSC));
		result = ADCL;
		result |= ADCH << 8;
		result = 1126400L / result; 			// Back-calculate in mV
		mes.fltVal = float(result / 1000.0);		// Voltage in Volt (float)
		txRadio();
		send4 = false;
	}
	if (send5) {						// Acknowledge on 'SET'
		mes.devID = 5;
		if (setAck) mes.intVal = 1; else mes.intVal = 0;// state (integer)
		send5 = false;
		txRadio();
	}
	if (send6) {						// Toggle on Buttonpress 
		mes.devID = 6;
		if (toggleOnButton) mes.intVal = 1; 		// read state of toggle flag
		else mes.intVal = 0;				// state (integer)
		send6 = false;
		txRadio();
	}
	if (send7) {						// timer interval
		mes.devID = 7;
		mes.intVal = TIMinterval;			// seconds (integer)
		send7 = false;
		txRadio();
	}
	if (send16) {						// state of Actuator 1
		mes.devID = 16;
		mes.intVal = ACT1State;				// state (integer)
		send16 = false;
		txRadio();
	}
	if (send40) {						// Binary input read
		mes.devID = 40;
		if (curState == LOW) mes.intVal = 1;					// state (integer)
		send40 = false;
		txRadio();
	}
	if (send48) {						// Temperature
		mes.devID = 48;
		temp = dht.readTemperature(true);
		mes.fltVal = temp;				// Degrees F (float)
		send48 = false;
		txRadio();
	}
	if (send49) {						// Humidity
		mes.devID = 49;
		hum = dht.readHumidity();
		mes.fltVal = hum;				// Percentage (float)
		send49 = false;
		txRadio();
	}
	if (send64) {
		mes.devID = 64;
		lightLevel = analogRead(lightLevel);
		mes.intVal = lightLevel;
		send64 = false;
		txRadio();
	}
	if (send92) {						// error message invalid device
		mes.intVal = mes.devID;
		mes.devID = 92;
		send92 = false;
		txRadio();
	}

}
//
//
//=======================		TXRADIO
//

void txRadio()						// Transmits the 'mes'-struct to the gateway
{
	if (radio.sendWithRetry(GATEWAYID, (const void*)(&mes), sizeof(mes)), 3, ACK_TIME)
#ifdef DEBUG
	{
		Serial.print(" message ");
		Serial.print(mes.devID);
		Serial.println(" sent...");
	}
	else
	{
		Serial.println("No connection...");
		Serial.println("tried to send");
		Serial.print(mes.devID);
		Serial.print(" ");
		Serial.println(mes.intVal);
	}
#endif
}	// end txRadio