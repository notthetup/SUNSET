/* SUNSET - Sapienza University Networking framework for underwater Simulation, Emulation and real-life Testing
 *
 * Copyright (C) 2012 Regents of UWSN Group of SENSES Lab <http://reti.dsi.uniroma1.it/SENSES_lab/>
 *
 * Authors: Roberto Petroccia - petroccia@di.uniroma1.it
 *          Daniele Spaccini - spaccini@di.uniroma1.it
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License as published
 * at http://creativecommons.org/licenses/by-nc-sa/3.0/legalcode
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANATBILITY or FITNESS FOR A PARTICULAR PURPOSE. See the Creative Commons
 * Attribution-NonCommercial-ShareAlike 3.0 Unported License for more details.
 *
 * You should have received a copy of the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
 * along with this program. If not, see <http://creativecommons.org/licenses/by-nc-sa/3.0/legalcode>.
 */


#include "sunset_evologics_v1_4.h"
#include "sunset_evologics_commands.h"

pthread_mutex_t mutex_evo_1_4_timer_rx;
pthread_mutex_t mutex_evo_1_4_timer_connection;

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_Evologics_v1_4Class : public TclClass 
{
public:
	
	Sunset_Evologics_v1_4Class() : TclClass("Sunset_Evologics_v1.4") {}
	
	TclObject* create(int argc, const char*const* argv) {
		
		return (new Sunset_Evologics_v1_4());
	}
	
} class_Sunset_Evologics_v1_4;

Sunset_Evologics_v1_4::Sunset_Evologics_v1_4()  : Sunset_Generic_Modem(), 
connectionTimer_(this), timeoutTimer_(this), timeoutDeliv_(this), 
timeoutBurstResp_(this), rttTimer_(this), rangingTimer_(this), rxTimer_(this)
{
	evo_conn = 0;
	EV_BROADCAST = 255;
	is_ranging = 0;
	rangingTime = 0.0;
	useBurst = 0;
	EV_USE_ACK = 0;
	tx_time = 0.0;
	
	pthread_mutex_init(&mutex_evo_1_4_timer_rx, NULL);
	pthread_mutex_init(&mutex_evo_1_4_timer_connection, NULL);
	
	bind("EV_BROADCAST", &EV_BROADCAST);
	bind("EV_USE_ACK", &EV_USE_ACK);
	bind("USE_EVO_BURST", &useBurst);
	bind("is_ranging", &is_ranging);
	bind("rangingTime", &rangingTime);
	
	range_dest -1;
	
	mac_slot = -1;
	deliveryRetry = 0;
	current_tx_power = -1;
	evoControl = 1;
	evoPower = 3;
	
	listenMode = true;
	
	already_started = 0;
}

/*! @brief This destructor closes the TCP connection.
 */

Sunset_Evologics_v1_4::~Sunset_Evologics_v1_4() 
{
	
	pthread_mutex_destroy(&mutex_evo_1_4_timer_rx);
	pthread_mutex_destroy(&mutex_evo_1_4_timer_connection);
	
	if ( close_connection() == false ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::~Sunset_Evologics_v1_4 Cannot close socket fd");
	}
}

/*!
 * 	@brief The start() function can be called from the TCL script to execute modem operations when the simulation starts.
 */

void Sunset_Evologics_v1_4::start()
{
	
	Sunset_Generic_Modem::start();
	
	if (sid != NULL) {
		
		sid_id = sid->register_module(getModuleAddress(), "EVO_MODEM_1.4", this);
		
		sid->define(getModuleAddress(), sid_id, "NODE_PROPAGATION_DELAY");
		sid->define(getModuleAddress(), sid_id, "TX_POWER");
		
		sid->provide(getModuleAddress(), sid_id, "NODE_PROPAGATION_DELAY");
		sid->provide(getModuleAddress(), sid_id, "TX_POWER");
		
		sid->subscribe(getModuleAddress(), sid_id, "TX_POWER");
	}
}

/*! @brief The startRanging() function is called when the user starts ranging operations using the evologics modem features.
 *	@param[in] dest It is the node to whom the ranging estimation is computed.
 *	@retval false If an error occurs.
 *	@retval true If the ranging operation is correctly started.
 */

bool Sunset_Evologics_v1_4::startRanging() 
{
	
	if (evo_conn == 0 || connectionTimer_.busy()) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::startRanging connection ERROR");
		
		return false;
	}
	
	if (is_ranging == false) { // the node does not support ranging
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::startRanging not set");
		
		return false;
	}
	
	if (getState() == EV_RANGING) { // already doing ranging 
		
		if (range_dest == -1) { // check correct range_dest
			
			setState(EV_IDLE);
			
			return false;
		}
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::startRanging running");
		
		return false;
	}
	
	send_data_imRTT();
}

/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script 
 *	@param[in] argc argc is a count of the arguments supplied to the command function.
 *	@param[in] argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_Evologics_v1_4::command( int argc, const char * const * argv ) 
{
	struct evo_setting cmd;
	Tcl& tcl = Tcl::instance();
	
	if ( argc == 5 ) {
		
		if (strcmp(argv[1], "setConnectionType") == 0) {
			
			/* The "setConnectionType" command sets all the parameter that will be used for the connection to the modem. */
			if (strcmp(argv[2], "tcp") == 0) {
				
				char socketIpAddress[EV_MAX_IP_LEN];
				int socketPort;
				
				memset(socketIpAddress, '\0', EV_MAX_IP_LEN);
				snprintf(socketIpAddress, EV_MAX_IP_LEN, "%s", argv[3]);
				
				Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Evologics_v1_4::command socketIpAddress %s", socketIpAddress);	
				socketPort = atoi(argv[4]);
				
				Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4 socketPort %d", socketPort);
				evo_conn = new Sunset_Evologics_Conn(socketIpAddress, socketPort);
				
				return TCL_OK;		      
			}
		}
	}
	else if ( argc == 3 ) {
		
		if ( strcmp(argv[1], "setEvoControl") == 0 ) {
			
			evoControl = atoi(argv[2]);
			
			return TCL_OK;
		}
		
		/* The "setTxPower" command sets the modem transmission power. */
		if ( strcmp(argv[1], "setTxPower") == 0 ) {
			
			evoPower = atoi(argv[2]);
			
			return TCL_OK;
		}
		
		/* The "setPromiscousMode" command sets the modem in promiscous mode. */
		if ( strcmp(argv[1], "setPromiscousMode") == 0 ) {
			
			cmd.type = EV_SET_PROMISCOUS;
			cmd.value = atoi(argv[2]);
			
			setting_cmd.push(cmd);
			
			return TCL_OK;
		}
		
		/* The "setBroadcast" command sets the modem broadcast address. */
		if ( strcmp(argv[1], "setBroadcast") == 0 ) {
			
			int bAddr = atoi(argv[2]);
			
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4 setBroadcast addr %d", bAddr);
			
			EV_BROADCAST = bAddr;
			
			return TCL_OK;
		}
		
		/* The "startRanging" command start the computation of the propagation delay to a destination node using the evologics modem features. */
		if ( strcmp(argv[1], "startRanging") == 0 ) {
			
			range_dest = atoi(argv[2]);
			
			if (range_dest < 0 || range_dest == getModuleAddress()) {
				
				Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4 startRanging ERROR dest %d", range_dest);
				
				return TCL_OK;
				
			}
			
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4 startRanging %d time %f", range_dest, rangingTime);
			
			startRanging();
			
			return TCL_OK;
		}
		
		/* If the "useBurst" is set to 0 then the instant messages are used. Otherwise if it is equal to 1 the burst messages are used. */
		if ( strcmp(argv[1], "useBurst") == 0 ) {
			
			useBurst = atoi(argv[2]);
			
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4 useBurst %d", useBurst);
			
			return TCL_OK;
		}
	}
	
	else if ( argc == 4 ) {
		
		/* The "startRanging" command start the computation of the propagation delay to a destination node using the evologics modem features. */
		if ( strcmp(argv[1], "startRanging") == 0 ) {
			
			range_dest = atoi(argv[2]);
			
			rangingTime = atof(argv[3]);
			
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4 startRanging %d time %f", range_dest, rangingTime);
			
			if (rangingTime <= 0.0 || range_dest < 0 || range_dest == getModuleAddress()) {
				
				Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4 startRanging ERROR dest %d time %f", range_dest, rangingTime);
				
				return TCL_OK;
				
			}
			
			startRanging();
			
			return TCL_OK;
		}
	}
	
	else if ( argc == 2 ) {
		
		if ( strcmp(argv[1], "exit_initial_state") == 0 ) {
			
			if ( getState() == EV_INITIAL_STATE ) {
				
				setState(EV_IDLE);
				
			}
			
			return TCL_OK;
		}
		
		if ( strcmp(argv[1], "stopRanging") == 0 ) {
			
			rangingTime = 0.0;
			
			range_dest = -1;
			
			if (getState() == EV_RANGING) {
				
				if (!(rangingTimer_.busy()) && !(timeoutTimer_.busy())) {
					
					setState(EV_IDLE);
					
				}
			}
			
			return TCL_OK;
		}
	}
	
	return Sunset_Generic_Modem::command( argc, argv );
}

/*!
 * 	@brief The startConnection() function is called when the connection to the modem has not been completed at the first try and re-tries are needed.
 * 	@retval 0 in case of error, the file descriptor fo the TCP connection otherwise.
 */

int Sunset_Evologics_v1_4::startConnection() {	
	
	listening = 1;
	
	int res = connect();
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::startConnection res  %d", res);
	
	return res;
	
}

/*!
 * 	@brief The stopConnection() function is called when the TCP connection to the modem has th be ended.
 * 	@retval 0 in case of error, 1 otherwise.
 */

int Sunset_Evologics_v1_4::stopConnection() {
	
	int res = evo_conn->close_connection();
	
	listening = 0;
	
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Evologics_v1_4::stopConnection res %d", res);
	
	return res;
}

/*!
 * 	@brief The getTransfertTime function computes the time needed to transfer a given number of bytes to an external device using the ethernet line.
 * 	@param[in] size Number of bytes to be transferred.
 * 	@retval The transfer delay
 */

double Sunset_Evologics_v1_4::getTransfertTime(int size) 
{
	
	double time = 0.0;
	
	time = (size * 8.0) / 1000000.0;
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::getTransfertTime size %d time %f", size, time);
	
	return time;
}

/*!
 * 	@brief The getTxTime function computes the time needed to transmit a given number of bytes.
 * 	@param[in] size Number of bytes to be transmitted.
 * 	@retval The transmission delay
 */

double Sunset_Evologics_v1_4::getTxTime(int size)
{
	
	if (dataRate < 0.0) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::getTxTime dataRate %f ERROR", dataRate);
		//		return 1;
		exit(1);
	}		
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::getTxTime size %d dataRate %f", size, dataRate);
	
	return (size * 8.0) / dataRate;
}

/*!
 * 	@brief The connect() function opens a TCP connection. \n
 *	It also starts the thread responsible for reading messages from the modem 
 * and sends all the settings parameters chosen to the modem.
 * 	@retval 0 in case of error, the file descriptor fo the TCP connection otherwise.
 */

int Sunset_Evologics_v1_4::connect() 
{
	
	int x = 0;
	bool res = false;
	struct evo_setting cmd;
	
	if (timeoutTimer_.busy()) {
		
		timeoutTimer_.stop();
	}
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::connect starting connection");
	
	res = start_connection();
	
	if (res && evo_conn != 0) {
		
		x = evo_conn->get_fd();
		
	} else {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::connect Cannot connect to %s:%d", evo_conn->get_ip(), evo_conn->get_port());
		
		pthread_mutex_lock(&mutex_evo_1_4_timer_connection);
		
		if (connectionTimer_.busy()) {
			
			connectionTimer_.stop();
		}
		
		connectionTimer_.start(EVO_TIMEOUT_CONNECTION);
		
		pthread_mutex_unlock(&mutex_evo_1_4_timer_connection);
		
		return 0;
	}
	
	if (evoControl) {
		
		setState(EV_SETTING);
		
	}
	else {
		
		if ( already_started == 0 ) {
			
			setState(EV_INITIAL_STATE);
		}
		else {
			
			setState(EV_IDLE);
		}
	}
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::connect connecting fd = %d", x);
	
	res = start_listener();
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::connect connecting start_listener");
	
	if (!res) {
		
		Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::connect CANNOT  start_listener");
		return 0;
	}
	
	if (evoControl) {
		
		cmd.type = EV_SET_ADDR;
		cmd.value = getModuleAddress();
		setting_cmd.push(cmd);
		
		cmd.type = EV_SET_POWER;
		cmd.value = evoPower;
		setting_cmd.push(cmd);
		
		cmd.type = EV_SET_PROMISCOUS;
		cmd.value = 1;
		setting_cmd.push(cmd);
		
		modem_setting();	
	}
	
	return x;
}

/*!
 * 	@brief The modem_setting() function sends the configuration settings to the modem.
 * 	@retval false In case of error.
 *	@retval true Otherwise.
 */

bool Sunset_Evologics_v1_4::modem_setting() 
{
	
	if ( setting_cmd.empty() == false ) {
		
		switch(setting_cmd.front().type) {
				
			case EV_SET_POWER:
				
				Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::checkSetting() setting modem power");					
				
				if ( set_modem_power(setting_cmd.front().value) == false ) {
					
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::modem_setting error setting power");
				
					return false;
				}
				
				break;
				
			case EV_SET_ADDR:
				
				Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::checkSetting() setting modem address");					
				
				if ( set_local_address(setting_cmd.front().value) == false ) {
					
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::modem_setting error setting address");					
					
					return false;
				}
				
				break;
				
			case EV_SET_PROMISCOUS:
				
				Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::checkSetting() setting modem promiscous mode");					
				
				if ( set_promiscous_mode(setting_cmd.front().value) == false ) {
					
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::modem_setting error setting promiscous mode");					
					
					return false;
				}
				
				break;
				
			default:
				break;
		}
		
	} else {
		
		if (timeoutTimer_.busy()) {
		
			timeoutTimer_.stop();
		}
		
		return true;
	}
	
	return false;
}

void Sunset_Evologics_v1_4::checkSetting()
{
	
	if (!(setting_cmd.empty()) && setting_cmd.front().type == EV_SET_POWER) {
		
		current_tx_power = setting_cmd.front().value;
		
		setTxPower(getModuleAddress(), current_tx_power, NOW);    		
	}
	
	setting_cmd.pop();
	
	if ( modem_setting() == true ) {
		
		Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::checkSetting() all settings done");					
		
		setState(EV_IDLE);
	}
	
	return;
}

/*!
 * 	@brief The set_local_address() function sets the modem local address.
 * 	@param[in] addr The modem local address to be set.
 * 	@retval false An error occured.
 *	@retval true Otherwise.
 */

bool Sunset_Evologics_v1_4::set_local_address(int addr) 
{
	
	char msg[64];
	
	memset(msg, 0x0, 64);
	
	snprintf(msg, 64, "%s%d%s", ATALn, addr, EV_EOC);
	
	setState(EV_SETTING);
	
	if ( evo_conn->write_data(msg, strlen(msg)) == false ) {
		
		return false;
	}
	
	if (timeoutTimer_.busy()) {
		
		timeoutTimer_.stop();
	}
	
	timeoutTimer_.start(EV_TIMEOUT_CMD);
	
	return true;
}

/*!
 * 	@brief The set_promiscous_mode() function sets the modem promiscous mode.
 * 	@param[in] value The promiscous mode value.
 * 	@retval false An error occured.
 *	@retval true Otherwise.
 */

bool Sunset_Evologics_v1_4::set_promiscous_mode(int value) 
{
	
	char msg[64];
	
	memset(msg, 0x0, 64);
	
	snprintf(msg, 64, "%s%d%s", ATRP, value, EV_EOC);
	
	setState(EV_SETTING);
	
	if ( evo_conn->write_data(msg, strlen(msg)) == false ) {
		
		return false;
	}
	
	if (timeoutTimer_.busy()) {
	
		timeoutTimer_.stop();
	}
	
	timeoutTimer_.start(EV_TIMEOUT_CMD);
	
	return true;
}

/*!
 * 	@brief The set_modem_power() function sets the modem transmission power. 
 *	Four possible values can be set [0-3]. 0 corresponds to the highest transmission power level and 3 to the lowest one.
 * 	@param[in] power The transmission power level.
 * 	@retval false An error occured.
 *	@retval true Otherwise.
 */

bool Sunset_Evologics_v1_4::set_modem_power(int power) 
{
	
	char msg[64];
	
	memset(msg, 0x0, 64);
	
	snprintf(msg, 64, "%s%d%s", ATLn, power, EV_EOC);
	
	setState(EV_SETTING);
	
	Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_Evologics_v1_4::set_modem_power  mando %s\n", msg);
	
	if ( evo_conn->write_data(msg, strlen(msg)) == false ) {
		
		return false;
	}
	
	if (timeoutTimer_.busy()) {
	
		timeoutTimer_.stop();
	}
	
	timeoutTimer_.start(EV_TIMEOUT_CMD);
	
	return true;
}


/*!
 * 	@brief The start_connection() opens the TCP connection.
 */

bool Sunset_Evologics_v1_4::start_connection() 
{
	
	bool res = false;
	
	res = evo_conn->open_connection();
	
	return res;
}

/*!
 * 	@brief The close_connection() closes the TCP connection.
 */

bool Sunset_Evologics_v1_4::close_connection() 
{
	listening = 0;
	
	return evo_conn->close_connection();
}

bool Sunset_Evologics_v1_4::start_listener() 
{
	
	pthread_t listener_thread;
	int ret = 0;
	
	ret = pthread_create(&listener_thread, NULL, thread_proxy_function_1_4, this);
	
	if ( ret ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::start_listener Cannot create thread - exiting");
		
		return false;
		
	}
	Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::start_listener  create thread - DONE");
	
	return true;
}

void *thread_proxy_function_1_4(void *_tgtObject) 
{
	
	pthread_detach(pthread_self());
	
	Sunset_Evologics_v1_4 *tgtObject = (Sunset_Evologics_v1_4*)_tgtObject;
	
	tgtObject->RxIterate(tgtObject->evo_conn);
	
	return (NULL);
}

/*!
 * 	@brief The resetTx() function clears all the information regarding the last transmission on-going. 
 *	It also stops the modem timer before the timeout.
 */

void Sunset_Evologics_v1_4::resetTx() 
{
	int check = 0;
	
	Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::resetTx");
	
	if (!pktTxList.empty()){
		
		Packet* p = getPktTxList();
		pktTxList.pop_front();
		check = 1;
	}
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Evologics_v1_4::resetTx exit");
	
	if (check) {
		
		resetTx();
	}
	
	return;
}

/*!
 * 	@brief The pktReceived() function is called when an Evologics data packet is received.
 * 	@param[in] buffer The payload of the Evologics data packet.
 * 	@param[in] len The length of the payload.
 */

void Sunset_Evologics_v1_4::pktReceived(char* buffer, int len) 
{
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::pktReceived sendUp");
	
	Sunset_Generic_Modem::pktReceived(buffer, len);
	
	return;	
}

/*!
 * 	@brief The pktReceived() function is called when an Evologics data packet is received including packet source nad destination information.
 * 	@param[in] src The source ID of the received message.
 * 	@param[in] dst The destination ID of the received message.
 * 	@param[in] buffer The payload of the Evologics data packet.
 * 	@param[in] len The length of the payload.
 */

void Sunset_Evologics_v1_4::pktReceived(int src, int dst, char* buffer, int len) 
{
	Packet* p;
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::pktReceived sendUp");
	
	if (dst == EV_BROADCAST) {
		
		dst = Sunset_Address::getBroadcastAddress();
	}
	
	p = Modem2Pkt(src, dst, buffer, len);
	
	if (p == 0) {
		
		rxAborted();
		return;
	}
	
	if (roundTripTime.find(src) != roundTripTime.end() && (roundTripTime[src]).first != -1.0) {
		
		SUNSET_HDR_CMN(p)->propagation_delay = (roundTripTime[src]).first / 2.0;
		SUNSET_HDR_CMN(p)->timestamp = (roundTripTime[src]).second;	
	}
	
	if (distanceToNode.find(src) != distanceToNode.end() && (distanceToNode[src]).first != -1.0) {
		
		SUNSET_HDR_CMN(p)->distance = (distanceToNode[src]).first;
		SUNSET_HDR_CMN(p)->timestamp = (distanceToNode[src]).second;
	}
	
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Evologics_v1_4::pktReceived sendUp");
	
	pktRxList.push_back(p);
	
	if (Sunset_Statistics::use_stat() && stat != NULL) {
		
		stat->logStatInfo(SUNSET_STAT_MODEM_RX_DONE, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");
	}
	
	sendUpPkt(p);
	
	return;	
}

/*!
 * 	@brief The pktWrite() function returns the buffer to be transmitted to the Evologics modem, corresponding to the ns-2 packet in input after the conversion 
 *	and the encoding operations.
 *	@param[in] p The packet to be converted.
 *	@param[out] len The length of the bytes array to be transmitted.
 * 	@retval The bytes array to be transmitted.
 */

char* Sunset_Evologics_v1_4::pktWrite(Packet* p, int& len) 
{
	int dataLen = 0;
	char* bufferData;
	
	bufferData = pkt2Modem(p, dataLen);
	
	if( dataLen <= 0 || bufferData == NULL ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::pktWrite no bufferData  ERROR");
		
		return NULL;
	}
	
	Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::pktWrite len %d", dataLen);
	len = dataLen;
	
	return bufferData;
}

/*!
 * 	@brief The sendDown() function converts the ns-2 packet into a stream of bytes and sends such stream to the Evologics modem.
 *	@param p The packet to be sent.
 */

void Sunset_Evologics_v1_4::sendDown(Packet *p)
{
	int len = 0;
	char* buffer;
	
	int dst = 0, src = 0;
	
	double ref_usec = 0;
	double delta = 0;
	double offset = 0.0;
	
	pktConverter_->getSrc(p, UW_PKT_MAC, src);
	pktConverter_->getDst(p, UW_PKT_MAC, dst);
	
	if (pktTxList.size() > 0) { //it should not happen - we do not buffer data, remove old packet
		
		resetTx();	
	}
	
	pktTxList.push_back(p);
	
	if (evo_conn == 0 || connectionTimer_.busy()) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::sendDown packet with connection ERROR");
		
		txAborted();
		
		return;
	}
	
	if (src != getModuleAddress()) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::sendDown packet with src %d ERROR", src);
		
		txAborted();
		
		return;
	}
	
	if (getState() == EV_RANGING) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::sendDown packet RANGING running");
		
		txAborted();
		
		return;
	}
	
	if ((u_int8_t)dst == Sunset_Address::getBroadcastAddress()) {
		
		dst = EV_BROADCAST;
	}
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Evologics_v1_4::sendDown packet with src %d dst %d MAC_BROADCAST %d EV_BROADCAST %d", src, dst, Sunset_Address::getBroadcastAddress(), EV_BROADCAST);
	
	
	if (getState() != EV_IDLE && getState() != EV_SETTING) {
		
		setState(EV_IDLE);
	}
	
	buffer = (char*)(pkt2Modem(p, len));
	
	if ( len <= 0 || buffer == NULL ) {
		
		txAborted();
		
		setState(EV_IDLE);
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::sendDown MALLOC ERROR");
		
		return;
	}
	
	for (int i = 0; i < len; i++) {
		
		Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::sendDown sending DOWN(%d) = %x", i, buffer[i]);
	}
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Evologics_v1_4::sendDown packet len %d is_ranging %d buffer %s check %d", len, is_ranging, (char*)buffer, strlen(buffer));
	
	if (useBurst) {
		
		if(!(send_data_burst(buffer, len, dst))) {
			
			setState(EV_IDLE);
			
			txAborted();
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::sendDown cannot write BURST data to the modem ERROR");
		}
	}
	else {
		if (EV_USE_ACK) {
			
			if(!(send_data_im(buffer, len, dst, (char*)EV_ACK))) {
				
				Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::sendDown cannot write data to the modem with ACK ERROR");
				
				setState(EV_IDLE);
				
				txAborted();
			}
			
		} else {
			
			if(!(send_data_im(buffer, len, dst, (char*)EV_NO_ACK))) {
				
				Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::sendDown cannot write data to the modem ERROR");
				
				setState(EV_IDLE);
				
				txAborted();
			}	
		}		
	}	
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Evologics_v1_4::sendDown packet len %d buffer %s check %d state %d", len, (char*)buffer, strlen(buffer), getState());
	
	free(buffer);
	
	return;
}


/*!
 * 	@brief The txDone() function is called when a packet transmission has been correctly completed.
 *	All the status and temporary data structures are cleared from the information 
 *	regarding the completed transmission.
 */

void Sunset_Evologics_v1_4::txDone() 
{
	
	Packet* p;
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Evologics_v1_4::txDone");
	
	if (pktTxList.empty()) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::txDone - " "Something is wrong with tx %d ERROR", (int)(pktTxList.size()));
		
		return;
	}
	
	if (timeoutTimer_.busy()) {

		timeoutTimer_.stop();
	}
	
	p = getPktTxList();
	
	pktTxList.pop_front();
	
	Modem2PhyEndTx(p);
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Evologics_v1_4::txDone tx %d", (int)(pktTxList.size()));
	
	return;
}

/*!
 * 	@brief The txAborted function is called when a packet transmission has been aborted because of a timeout or an 
 *	error message received from the modem. All the status and temporary data structures are cleared from the information regarding the aborted transmission.
 */

void Sunset_Evologics_v1_4::txAborted() 
{
	Packet* p;
	int dst = 0, src = 0;
	
	if (pktTxList.empty()) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::txAborted - " "Something is wrong with pktList tx %d ERROR", (int)(pktTxList.size()));
		
		return;
	}
	
	if (timeoutTimer_.busy()) {
		
		timeoutTimer_.stop();
	}
	
	p = getPktTxList();
	
	pktTxList.pop_front();
	
	Modem2PhyTxAborted(p);
	
	Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::txAborted tx %d", (int)(pktTxList.size()));
	
	return;
}

/*!
 * 	@brief The rxAborted() function is used when an error occurs during the packet reception. 
 *	It creates an ns-2 packet with the error field of the common header set to 1, and notify such event to the upper layer.
 *	The information regarding the last reception on-going are cleared.
 */

void Sunset_Evologics_v1_4::rxAborted() 
{
	Sunset_Generic_Modem::rxAborted();	
	
	return;
}

/*!
 * 	@brief The RxIterate() function continuosly listens for incoming messages from the modem.
 */

void Sunset_Evologics_v1_4::RxIterate(Sunset_Evologics_Conn *p) 
{	
	char recvb[EV_BUFSIZE];
	int len = 0;
	
	int check = 0;
	
	while(listening) {
		
		pair<char*, int > rp;
		
		memset(recvb, 0x0, EV_BUFSIZE);
		
		len = evo_conn->read_data(recvb, EV_BUFSIZE);
		
		Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Evologics_v1_4::RxIterate len %d %s", len, recvb);
		
		switch(len) {
				
			case EV_READ_NO_CLIENT:
				
				Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::RxIterate Client read 0 bytes");
				
				check = 1;
				
				break;
				
			case EV_READ_ERR:
				
				Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::RxIterate Read error");
				
				check = 1;
				
				break;
				
			case EV_READ_BUF_MAX:
				
				Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::RxIterate Buffer limit reached");
				
				setState(EV_IDLE);
				
				continue;
				
			default:
				break;
		}
		
		Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Evologics_v1_4::RxIterate state %d IDLE %d received %s len %d", getState(), getState() == EV_IDLE, recvb, len);
		
		if (check) {
			
			break;			
		}
		
		rp.first = (char*) malloc (len+1);
		
		if ( rp.first == NULL ) {
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::RxIterate rp.first NULL EXIT");
			
			exit(1);
		}
		
		rp.second = len;
		
		memset (rp.first, 0x0, len+1);
		memcpy(rp.first, recvb, len);
		
		pthread_mutex_lock(&mutex_evo_1_4_timer_rx);
		
		rx_info.push_back (rp);
		
		pthread_mutex_unlock(&mutex_evo_1_4_timer_rx);
		
		if (!(rxTimer_.busy())) {
			
			rxTimer_.start(0.0001); // post event to notify the main thread ro process the received information	
		}
	}
	
	Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_Evologics_v1_4::RxIterate exit");
	
	listening = 0;

	already_started = 1;
	
	pthread_mutex_lock(&mutex_evo_1_4_timer_connection);
	
	if (connectionTimer_.busy()) {
		
		connectionTimer_.stop();
	}
	
	connectionTimer_.start(0.001);
	
	pthread_mutex_unlock(&mutex_evo_1_4_timer_connection);
	
	pthread_exit(NULL);
	
	return;
}


bool Sunset_Evologics_v1_4::processRxInfo() 
{
	pair<char*, int > rp;
	int len = 0;
	char recvb[EV_BUFSIZE]; 
	
	pthread_mutex_lock(&mutex_evo_1_4_timer_rx);
	
	if (rx_info.empty()) {
	
		pthread_mutex_unlock(&mutex_evo_1_4_timer_rx);
		
		return false;
	} 
	
	rp = rx_info.front();
	
	rx_info.pop_front();
	
	pthread_mutex_unlock(&mutex_evo_1_4_timer_rx);
	
	len = rp.second;
	
	memset(recvb, 0x0, EV_BUFSIZE);
	memcpy(recvb, rp.first, rp.second);    
	
	free(rp.first);
	
	if (getState() == EV_RANGING) { 
		
		if ( strcmp(recvb, EV_OK) == 0 || strcmp(recvb, EV_OK2) == 0 ) {
			
			return true;
		}
		
		if (strncmp(recvb, EV_DELIVEREDIM, strlen(EV_DELIVEREDIM)) == 0 ) { 
			
			if (rttTimer_.busy()) {
				
				rttTimer_.stop();
			}
			
			rttTimer_.start(EV_ATT_REQUEST_TIME_1_4);
			
			return true;
			
		}
		
		if (strncmp(recvb, EV_FAILED, strlen(EV_FAILED)) == 0 ||
		    strcmp(recvb, EV_BUSY) == 0 || strcmp(recvb, EV_ERROR_WRONG_FORMAT) == 0 ||
		    strncmp(recvb, EV_RECVIM, strlen(EV_RECVIM)) == 0 ||
		    strncmp(recvb, EV_RECV, strlen(EV_RECV)) == 0 ||
		    strncmp(recvb, EV_RECV, strlen(EV_FAILEDIM)) == 0) { 
			
			if (timeoutTimer_.busy()) {
				
				timeoutTimer_.stop();
			}
			
			if (rangingTimer_.busy()) {
				
				rangingTimer_.stop();
			}
			
			if (rangingTime > 0.0 && range_dest != -1 ) {
				
				Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::processRxInfo RANGING startTimer");
				
				setState(EV_IDLE);
				
				rangingTimer_.start(rangingTime, range_dest);
				
			} else {
				
				range_dest = -1;
				setState(EV_IDLE);
			}
			
			return true;
		}
		
		if (timeoutTimer_.busy()) {
			
			timeoutTimer_.stop();
		}	
		
		Sunset_Debug::debugInfo(2, getModuleAddress(), "(%f) (%d) (%d) _DELAY_ %s us", Sunset_Utilities::get_epoch(), getModuleAddress(), range_dest, recvb);
		
		setDelayToNode(range_dest, atof(recvb));
		
		if (rangingTime == 0.0) {
			
			range_dest = -1;
			setState(EV_IDLE);
			
		} else {
			
			if (!(rangingTimer_.busy())) {
				
				if (rangingTime > 0.0 && range_dest != -1 ) {
					
					Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::processRxInfo RANGING startTimer");
					
					setState(EV_IDLE);
					
					rangingTimer_.start(rangingTime, range_dest);
					
				} else {
					
					range_dest = -1;
					setState(EV_IDLE);	
				}
			}
		} 
		
		return true;
		
	}
	
	if ( strcmp(recvb, EV_OK) == 0 || strcmp(recvb, EV_OK2) == 0 ) {
		
		/* checking if the setting command has been executed */
		
		Packet* p;
		
		if ( getState() == EV_INITIAL_STATE ) {
			
			return true;
		}
		
		if ( getState() == EV_SETTING ) {
			
			if (timeoutTimer_.busy()) {
			
				timeoutTimer_.stop();
			}
			
			checkSetting();
			
			return true;
		}
		
		p = getPktTxList();	
		
		/* checking if the instant message has been sent. */
		
		if ( getState() == EV_DATA_IM  && EV_USE_ACK == 0) {
			
			if (timeoutTimer_.busy()) {
				
				timeoutTimer_.stop();
			}
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Evologics_v1_4::processRxInfo Sending status request");
			
			setState(EV_WAIT_DATA_STATUS);
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Evologics_v1_4::processRxInfo Waiting status response");
			
			if (timeoutDeliv_.busy()) {
				
				timeoutDeliv_.stop();
			}
			
			if (deliveryRetry == 0 && tx_time > 0.0) {
				
				Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Evologics_v1_4::processRxInfo delivery timeout tx_time %f deliveryRetry %d", tx_time, deliveryRetry);
				
				timeoutDeliv_.start(tx_time);
			} 
			else {
				
				Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Evologics_v1_4::processRxInfo delivery timeout %f deliveryRetry %d", EV_TIMEOUT_DLV, deliveryRetry);
				
				timeoutDeliv_.start(EV_TIMEOUT_DLV);		
			}
			
			return true;
						
		}
		
		/* checking if the data burst has been sent. */
		
		if ( getState() == EV_DATA_BURST ) {
			
			if (timeoutBurstResp_.busy()) {
				
				timeoutBurstResp_.stop();
			}
			
			timeoutBurstResp_.start(EV_TIMEOUT_BURSTRESP);
			
			setState(EV_WAIT_BURST_RESP);
			
			return true;
		}
		
	} else	if (strncmp(recvb, EV_RECVFAILED, strlen(EV_RECVFAILED)) == 0 ) { 
		
		if ( getState() == EV_INITIAL_STATE ) {
			
			return true;
		}
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::processRxInfo EV_RECVFAILED");
		
		rxFailed(recvb+strlen(EV_RECVFAILED)+1, len - (strlen(EV_RECVFAILED)+1));
		
		return true;
		
	} else if (strcmp(recvb, EV_BUSY) == 0 || strcmp(recvb, EV_ERROR_WRONG_FORMAT) == 0 ) {
		
		/* if the modem is busy or the command format is wrong.. */
		
		if ( getState() == EV_INITIAL_STATE ) {
			
			return true;
		}
		
		if ( getState() == EV_SETTING ) {
			
			if (timeoutTimer_.busy()) {
				
				timeoutTimer_.stop();
			}
			
			modem_setting();
			
			return true;
		}
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::processRxInfo Waiting status BUSY");
		
		setState(EV_IDLE);
		
		txAborted();
		
	} else if ( strncmp(recvb, EV_RECVIM, strlen(EV_RECVIM)) == 0 ) {
		
		if ( getState() == EV_INITIAL_STATE ) {
			
			return true;
		}
		
		if ( getState() == EV_SETTING ) {
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::processRxInfo2 SETTING STATE");
			
			return true;	
		}
		
		/* checking if the instant message has been sent. */
		
		
		if ( getState() == EV_DATA_IM ) {
			
			
			if (timeoutTimer_.busy()) {
				
				timeoutTimer_.stop();
			}
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::processRxInfo2 TX STATE", getState());
			
			setState(EV_IDLE);
			
			deliveryRetry = 0;
			
			txAborted();
		}
		
		/* checking if the data burst has been sent. */
		
		if ( getState() == EV_DATA_BURST ) {
			
			if (timeoutBurstResp_.busy()) {
				
				timeoutBurstResp_.stop();
			}
			
			if (timeoutTimer_.busy()) {
				
				timeoutTimer_.stop();
			}
			
			if (timeoutDeliv_.busy()) {
				
				timeoutDeliv_.stop();
			}
			
			deliveryRetry = 0;
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::processRxInfo2 TX STATE", getState());
			
			setState(EV_IDLE);
			
			txAborted();
			
		}
		
		if ( getState() == EV_WAIT_DATA_STATUS) {
			
			if (timeoutTimer_.busy()) {
				
				timeoutTimer_.stop();
			}
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::processRxInfo2 TX STATE", getState());
			
			setState(EV_IDLE);
			
			deliveryRetry = 0;
			
			txAborted();
		}
		
		setState(EV_IDLE);
		
		rxDoneIm(recvb+strlen(EV_RECVIM)+1, len - (strlen(EV_RECVIM)+1));			
		
		return true;
	}
	
	else if ( strncmp(recvb, EV_RECV, strlen(EV_RECV)) == 0 ) {
		
		if ( getState() == EV_INITIAL_STATE ) {
			
			return true;
		}
		
		if ( getState() == EV_SETTING ) {
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::processRxInfo3 SETTING STATE");
			
			return true;	
		}
		
		/* checking if the instant message has been sent. */
		
		
		if ( getState() == EV_DATA_IM ) {
			
			if (timeoutTimer_.busy()) {
				
				timeoutTimer_.stop();
			}
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::processRxInfo3 TX STATE", getState());
			
			setState(EV_IDLE);
			
			deliveryRetry = 0;
			
			txAborted();
		}
		
		/* checking if the data burst has been sent. */
		
		if ( getState() == EV_DATA_BURST ) {
			
			if (timeoutBurstResp_.busy()) {
				
				timeoutBurstResp_.stop();
			}
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::processRxInfo3 TX STATE", getState());
			
			setState(EV_IDLE);
			
			txAborted();
		}
		
		if ( getState() == EV_WAIT_DATA_STATUS) {
			
			if (timeoutDeliv_.busy()) {
				
				timeoutDeliv_.stop();
			}
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::processRxInfo3 TX STATE", getState());
			
			setState(EV_IDLE);
			
			deliveryRetry = 0;
			
			txAborted();
		}
		
		setState(EV_IDLE);
		
		rxDoneBurst(recvb+strlen(EV_RECV)+1, len - (strlen(EV_RECV)+1));
		
		Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Evologics_v1_4::processRxInfo RX_DONE_BURST");
		
		return true;
	}
	else if ( getState() == EV_WAIT_DATA_STATUS && 
	
		strncmp(recvb, EV_DELIVERING, strlen(EV_DELIVERING)) == 0 ) {
		
		if (timeoutDeliv_.busy()) {
			
			timeoutDeliv_.stop();
		}
		
		if (deliveryRetry == 0 && tx_time > 0.0) {
			
			Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Evologics_v1_4::processRxInfo delivery timeout tx_time %f deliveryRetry %d", tx_time, deliveryRetry);
			
			timeoutDeliv_.start(tx_time);
		} 
		else {
			
			Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Evologics_v1_4::processRxInfo delivery timeout %f deliveryRetry %d", EV_TIMEOUT_DLV, deliveryRetry);
			
			timeoutDeliv_.start(EV_TIMEOUT_DLV);
		}
		
		
	} else if ( getState() == EV_WAIT_DATA_STATUS && 
		   
		strncmp(recvb, EV_EMPTY, strlen(EV_EMPTY)) == 0 ) {	
		
		if (timeoutDeliv_.busy()) {
			
			timeoutDeliv_.stop();
		}
		
		deliveryRetry = 0;
		
		Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Evologics_v1_4::TX_DONE IDLE");
		
		setState(EV_IDLE);
		
		txDone();
		
	} 
	else if ( getState() == EV_WAIT_DATA_STATUS && 
		   strncmp(recvb, EV_FAILED, strlen(EV_FAILED)) == 0 ) {	
		
		if (timeoutDeliv_.busy()) {
			
			timeoutDeliv_.stop();
		}
		
		deliveryRetry = 0;
		
		Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Evologics_v1_4::TX_ABORTED IDLE");
				
		setState(EV_IDLE);				

		txAborted();
	}
	else if ( getState() == EV_WAIT_BURST_RESP && 
		 strncmp(recvb, EV_DELIVERED, strlen(EV_DELIVERED)) == 0 ) {
		
		if (timeoutBurstResp_.busy()) {
			
			timeoutBurstResp_.stop();
		}
		
		Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Evologics_v1_4::TX_DONE2 IDLE");
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::processRxInfo3 TX STATE", getState());
	
		setState(EV_IDLE);	
	
		txDone();
	}
	else if ( getState() == EV_WAIT_BURST_RESP && 
		 strncmp(recvb, EV_FAILED, strlen(EV_FAILED)) == 0 ) {
		
		if (timeoutBurstResp_.busy()) {
			
			timeoutBurstResp_.stop();
		}
		
		Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Evologics_v1_4::TX_ABORTED2 IDLE");
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::processRxInfo3 TX STATE", getState());
		
		setState(EV_IDLE);
		
		txAborted();
	}
	else if ( strncmp(recvb, EV_DELIVEREDIM, strlen(EV_DELIVEREDIM)) == 0 && EV_USE_ACK == 1) {
		
		if (timeoutDeliv_.busy()) {
			
			timeoutDeliv_.stop();
		}
		
		Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Evologics_v1_4::TX_DONE IDLE");
		
		setState(EV_IDLE);
	
		txDone();
		
		Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Evologics_v1_4::processRxInfo DELIVEREDIM %s", recvb);
		
	}
	else if ( strncmp(recvb, EV_FAILEDIM, strlen(EV_FAILEDIM)) == 0 && EV_USE_ACK == 1) {
		
		if (timeoutDeliv_.busy()) {
			
			timeoutDeliv_.stop();
		}
		
		Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Evologics_v1_4::TX_ABORTED IDLE");
		
		setState(EV_IDLE);
		
		txAborted();
		
		Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Evologics_v1_4::processRxInfo DELIVEREDIM %s", recvb);
	}
	else {
		
		cout << recvb;
	}
	
	return true;
}

/*!
 * 	@brief The setDelayToNode() function is called to store information about the estimated propagation delay and distance to a given node.
 *	@param[in] node The addressed node.
 *	@param[in] delay_usec The estimated propagation delay to node in microsec.
 */

void Sunset_Evologics_v1_4::setDelayToNode(int node, double delay_usec) 
{
	double delay = delay_usec  /1000000.0;
	pair<double, double> delayPair;
	pair<double, double> distancePair;
	
	delayPair.first = delay * 2.0;
	delayPair.second = NOW; //Sunset_Utilities::get_epoch();
	roundTripTime[node] = delayPair;
	
	distancePair.first = delay * 1500.0;
	distancePair.second = NOW;
	distanceToNode[node] = distancePair;
	
	if (sid == NULL) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::setDelayToNode DISPATCHER NOT DEFINED");
		
		return;
	}
	
	notified_info ni1;
	ni1.node_id = node;
	ni1.info_time = NOW;
	ni1.info_name = "NODE_PROPAGATION_DELAY";
	
	if ( sid->assign_value(&delay, &ni1, sizeof(double)) == false) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::setDelayToNode ERROR ASSIGINING INFO %s", (ni1.info_name).c_str());
		
		return;
		
	} 
	
	if ( sid->set(getModuleAddress(), sid_id, ni1) == 0 ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::setDelayToNode PROVIDING INFO %s NOT DEFINED", (ni1.info_name).c_str());
		
	}
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Evologics_v1_4::setDelayToNode node %d delay %f distance %f", node, delay, distanceToNode[node].first);
}


/*!
 * 	@brief The checkDeliveryStatus() function sends an "AT?DI" command to check if the instant message has been sent.
 */

bool Sunset_Evologics_v1_4::checkDeliveryStatus() 
{
	
	char msg[EV_BUFSIZE];
	int total_len = 0;
	
	memset(msg, 0x0, EV_BUFSIZE);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::checkDeliveryStatus");
	
	snprintf(msg, EV_BUFSIZE, "%s%s", ATDI, EV_EOC);
	
	total_len = strlen(msg);
	
	if ( evo_conn->write_data(msg, total_len) == false ) {
		
		return false;
	}
	
	return true;
}

/*!
 * 	@brief The send_data_im() function creates an instant message and sends it to the modem.
 *	@param[in] data The instant message payload.
 *	@param[in] length The payload length.
 *	@param[in] device The destination id (device).
 *	@param[in] flag The instant message flag (ack or noack).
 *	@retval true All is done.
 *	@retval false An error occured.
 */

bool Sunset_Evologics_v1_4::send_data_im(char *data, int length, int device, char *flag) 
{
	
	char msg[EV_BUFSIZE];
	
	int res = 0;
	int header_len = 0;
	int total_len = 0;
	Packet* p;
	
	if ( getState() != EV_IDLE ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::send_data_im STATE ERROR %d", getState());
		
		return false;
	}
	
	if (length > EV_PACKET_LENGHT_IM) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::send_data_im length %d > EV_PACKET_LENGHT_IM ERROR", length);
		
		return false;
	}
	
	memset(msg, 0x0, EV_BUFSIZE);
	
	if ( device < 1 || device > EV_BROADCAST ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::send_data_im Wrong destination id %d", device);
		
		return false;	
	}
	
	p = getPktTxList();
	
	snprintf(msg, EV_BUFSIZE, "%s,%d,%d,%s,", AT_SENDIM, length, device, flag);
	header_len = strlen(msg);
	
	memcpy(msg + header_len, data, length);
	memcpy(msg + header_len + length, EV_EOC, strlen(EV_EOC));
	
	total_len = header_len + length + strlen(EV_EOC);
	
	setState(EV_DATA_IM);
	
	if (timeoutTimer_.busy()) {
	
		timeoutTimer_.stop();
	}
	
	timeoutTimer_.start(EV_TIMEOUT_CMD);
	
	Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_Evologics_v1_4::TX_START");
	Sunset_Debug::debugInfo(-1, getModuleAddress(), "SEND IM (slot,%d,power,%d,size,%d) --%s,%d,%d,%s--", mac_slot, current_tx_power, length, AT_SENDIM, length, device, flag);
	
	tx_time = getTxTime(length) + EV_TX_BASIC_TIME;
	
	if ( evo_conn->write_data(msg, total_len) == false ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::send_data_im error writing");
		
		setState(EV_IDLE);
		
		tx_time = 0.0;
		
		timeoutTimer_.stop();
		
		return false;
	}
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Evologics_v1_4::send_data_im exit state %d total_len %d length %d tx_time %f", getState(), total_len, length, tx_time);
	
	return true;
}

/*!
 * 	@brief The send_data_imRTT() function creates an instant message and sends it to the modem when computing the Round trip time using modem extimation.
 *	@retval true All is done.
 *	@retval false An error occured.
 */
bool Sunset_Evologics_v1_4::send_data_imRTT() 
{
	char data[EV_RANGING_SIZE];
	char msg[EV_BUFSIZE];
	
	int res = 0;
	int header_len = 0;
	int total_len = 0;
	Packet* p;
	
	if (getState() != EV_IDLE && getState() != EV_RANGING) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::send_data_imRTT busy");
		
		if (rangingTime > 0.0) {
			
			if (rangingTimer_.busy()) {
				
				rangingTimer_.stop();
			}
			
			if (rangingTime > 0.0 && range_dest != -1 ) {
				
				rangingTimer_.start(rangingTime, range_dest);		
			}
			else {
				
				range_dest = -1;
			}	      
		}
		
		return false;
	}
	
	if (range_dest == -1) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::send_data_imRTT range may be stopped");
		
		return false;
	}
	
	setState(EV_RANGING);
	
	data[0] = 'a';
		
	if (EV_RANGING_SIZE > EV_PACKET_LENGHT_IM) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::send_data_imRTT length %d > EV_PACKET_LENGHT_IM ERROR", EV_RANGING_SIZE);
		
		return false;
	}
	
	memset(msg, 0x0, EV_BUFSIZE);
	
	if ( range_dest < 1 || range_dest > EV_BROADCAST ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::send_data_imRTT Wrong destination id", range_dest);
		
		return false;
		
	}
	
	snprintf(msg, EV_BUFSIZE, "%s,%d,%d,%s,", AT_SENDIM, EV_RANGING_SIZE, range_dest, (char*)EV_ACK);
	
	header_len = strlen(msg);
	
	memcpy(msg + header_len, data, EV_RANGING_SIZE);
	memcpy(msg + header_len + EV_RANGING_SIZE, EV_EOC, strlen(EV_EOC));
	
	total_len = header_len + EV_RANGING_SIZE + strlen(EV_EOC);
	
	if ( evo_conn->write_data(msg, total_len) == false ) {
		
		if (rangingTime == 0.0) {
			
			range_dest = -1;
			setState(EV_IDLE);
			
		} else {
			
			if (rangingTimer_.busy()) {
				
				rangingTimer_.stop();
			}
			
			if (rangingTime > 0.0 && range_dest != -1 ) {
				
				setState(EV_IDLE);
				
				rangingTimer_.start(rangingTime, range_dest);
			}
			else {
				
				range_dest = -1;
				
				setState(EV_IDLE);		
			}	      
		}
			
		return false;
	}
	
	if (timeoutTimer_.busy()) {
		
		timeoutTimer_.stop();
	}
	
	timeoutTimer_.start(2 * EV_TIMEOUT_CMD);
	
	return true;
	
}

/*!
 * 	@brief The sendRTT_command() function is called when the round trip time to another node has to be estimated using the modem capabilities.
 *	@retval true All is done.
 *	@retval false An error occured.
 */


bool Sunset_Evologics_v1_4::sendRTT_command() 
{
	
	char msg[EV_BUFSIZE];
	
	int res = 0;
	int header_len = 0;
	int total_len = 0;
	
	if ( getState() != EV_RANGING ) {
		
		Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Evologics_v1_4::sendRTT_command STATE ERROR %d", getState());
		
		return false;
	}
	
	memset(msg, 0x0, EV_BUFSIZE);
	
	snprintf(msg, EV_BUFSIZE, "%s", ATT);
	header_len = strlen(msg);
	memcpy(msg + header_len, EV_EOC, strlen(EV_EOC));
	
	total_len = header_len + strlen(EV_EOC);
	
	if ( evo_conn->write_data(msg, total_len) == false ) {
		
		setState(EV_IDLE);
		
		return false;
	}
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Evologics_v1_4::sendRTT_command DONE");
	
	if (timeoutTimer_.busy()) {
	
		timeoutTimer_.stop();
	}
	
	timeoutTimer_.start(EV_TIMEOUT_CMD);
	
	return true;
	
}

/*!
 * 	@brief The send_data_burst() function creates a data burst message and sends it to the modem.
 *	@param[in] data The data burst payload.
 *	@param[in] length The payload length.
 *	@param[in] device The destination id (device).
 *	@param[in] flag The instant message flag (ack or noack).
 *	@retval true All is done.
 *	@retval false An error occured.
 */

bool Sunset_Evologics_v1_4::send_data_burst(char *data, int length, int device) 
{
	
	char msg[EV_BUFSIZE];
	
	int res = 0;
	int header_len = 0;
	int total_len = 0;
	Packet* p;
	
	if ( getState() != EV_IDLE ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::send_data_im STATE ERROR %d", getState());
		
		return false;
	}
	
	if (length > EV_PACKET_LENGHT_IM) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::send_data_im length %d > EV_PACKET_LENGHT_IM ERROR", length);
		
		return false;
	}
	
	memset(msg, 0x0, EV_BUFSIZE);
	
	if ( device < 1 || device > EV_BROADCAST ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::send_data_im Wrong destination id %d", device);
		
		return false;
		
	}
	
	p = getPktTxList();
	
	snprintf(msg, EV_BUFSIZE, "%s,%d,%d,", AT_SEND, length, device);
	
	header_len = strlen(msg);
	
	memcpy(msg + header_len, data, length);
	memcpy(msg + header_len + length, EV_EOC, strlen(EV_EOC));
	
	total_len = header_len + length + strlen(EV_EOC);
	
	setState(EV_DATA_BURST);
	
	if ( evo_conn->write_data(msg, total_len) == false ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::send_data_burst error writing");
		
		setState(EV_IDLE);		
		
		return false;
	}
	
	if (timeoutTimer_.busy()) {
	
		timeoutTimer_.stop();
	}
	
	timeoutTimer_.start(EV_TIMEOUT_CMD);
	
	return true;
}

/*!
 * 	@brief The rxDoneIm() function is invoked when an instant message is received.
 *	@param[in] data The instant message payload.
 *	@param[in] len The payload length.
 */

void Sunset_Evologics_v1_4::rxDoneIm(char *data, int len) 
{
	
	char *token;
	struct im_struct temp_im;
	char * aux = (char*) malloc (len+1);
	
	memset(temp_im.data, 0x0, EV_BUFSIZE);
	memset(temp_im.flag, 0x0, EV_FLAG_STR_SIZE+1);
	char* aux_mem;
	if ( aux == NULL ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneIm MALLOC ERROR");
		
		exit(1);
	}	
	
	memset(aux, '\0', len+1);
	memcpy(aux, data, len);
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Evologics_v1_4::RX_DONE IM");
	
	
	// <length>,<source address>,<destination address>,<flag>,<bitrate>,<rms>,<integrity>,
	//       <propagation time>,<velocity>,<data>
	// ex. EV_RECVIM,4,1,3,ack,3.0,1.0,2.0,3.4,5.3,ciaocciaocc
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneIm %s len %d - lenAll %d", data, strlen(data), len);
	
	token = strtok_r(data, ",", &aux_mem);
	
	if (token == NULL) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneIm  ERROR read token 1");
	
		free(aux);
		
		rxAborted();
		
		return;
	}
	
	temp_im.length = atoi(token);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneIm temp_im.length %d", temp_im.length);
	
	token = strtok_r(NULL, ",", &aux_mem);
	
	if (token == NULL) {
	
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneIm  ERROR read token 2");
	      	
		free(aux);
		
		rxAborted();
		
		return;
	}
	
	temp_im.source = atoi(token);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneIm temp_im.source %d", temp_im.source);
	
	token = strtok_r(NULL, ",", &aux_mem);
	
	if (token == NULL) {
	
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneIm  ERROR read token 3");
	      	
		free(aux);
		
		rxAborted();
		
		return;
	}
	
	temp_im.destination = atoi(token);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneIm temp_im.destination %d", temp_im.destination);
	
	token = strtok_r(NULL, ",", &aux_mem);
	
	if (token == NULL) {
	
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneIm  ERROR read token 4");
	      	
		free(aux);
		
		rxAborted();
		
		return;
	}
	
	strncpy(temp_im.flag, token, strlen(token));
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneIm temp_im.flag %s", temp_im.flag);
	
	token = strtok_r(NULL, ",", &aux_mem);
	
	if (token == NULL) {
	
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneIm  ERROR read token 5");
	      	
		free(aux);
		
		rxAborted();
		
		return;
	}
	
	temp_im.bitrate = atof(token);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneIm temp_im.bitrate %f", temp_im.bitrate);
	
	token = strtok_r(NULL, ",", &aux_mem);
	
	if (token == NULL) {
	
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneIm  ERROR read token 6");
	      	
		free(aux);
		
		rxAborted();
		
		return;
	}
	
	temp_im.rms = atof(token);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneIm temp_im.rms %f", temp_im.rms);
	
	token = strtok_r(NULL, ",", &aux_mem);
	
	if (token == NULL) {
	
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneIm  ERROR read token 7");
	      	
		free(aux);
		
		rxAborted();
		
		return;
	}
	
	temp_im.integrity = atof(token);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneIm temp_im.integrity %f", temp_im.integrity);
	
	token = strtok_r(NULL, ",", &aux_mem);
	
	if (token == NULL) {
	
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneIm  ERROR read token 8");
	      	
		free(aux);
		
		rxAborted();
		
		return;
	}
	
	temp_im.prop_time = atof(token);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneIm temp_im.prop_time %f", temp_im.prop_time);
	
	token = strtok_r(NULL, ",", &aux_mem);
	
	if (token == NULL) {
	
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneIm  ERROR read token 9");
	      	
		free(aux);
		
		rxAborted();
		
		return;
	}
	
	temp_im.velocity = atof(token);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneIm temp_im.velocity %f", temp_im.velocity);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneIm beforeData len %d temp_im.length %d", 
				len, temp_im.length);
	
	memcpy(temp_im.data, (aux + len - temp_im.length), temp_im.length);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneIm temp_im.data %s", temp_im.data);
	
	for (int i = 0; i < temp_im.length; i++) {
		
		Sunset_Debug::debugInfo(4, getModuleAddress(), "dat(%d) = %x", i, temp_im.data[i]);
	}
	
	Sunset_Debug::debugInfo(-1, getModuleAddress(), "RX IM (slot,%d,power,%d,size,%d) --EV_RECVIM,%d,%d,%d,%s,%f,%f,%f,%f,%f--", mac_slot, current_tx_power, temp_im.length, temp_im.length, temp_im.source, temp_im.destination, temp_im.flag, temp_im.bitrate, temp_im.rms, temp_im.integrity, temp_im.prop_time, temp_im.velocity);
	
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneIm Received EV_RECVIM,%d,%d,%d,%s,%f,%f,%f,%f,%f,%s ", temp_im.length, temp_im.source, temp_im.destination, temp_im.flag, temp_im.bitrate, temp_im.rms, temp_im.integrity, temp_im.prop_time, temp_im.velocity, temp_im.data);
	
	if (is_ranging == false || strcmp(temp_im.flag, EV_ACK) != 0 || temp_im.length != EV_RANGING_SIZE) {
		
		pktReceived(temp_im.source, temp_im.destination, temp_im.data, temp_im.length);
	}
	
	free(aux);
	
	return;	
}

/*!
 * 	@brief The rxFailed() function is invoked when the modem cannot decode successfully the received packet.
 *	@param[in] data The instant message payload.
 *	@param[in] len The payload length.
 */

void Sunset_Evologics_v1_4::rxFailed(char *data, int len) 
{
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::rxFailed %s len %d - lenAll %d", data, strlen(data), len);
	
	return;
}


/*!
 * 	@brief The rxDoneBurst() function is invoked when a data burst is received.
 *	@param[in] data The data burst payload.
 *	@param[in] len The payload length.
 */

void Sunset_Evologics_v1_4::rxDoneBurst(char *data, int len) 
{
	
	char *token;
	struct burst_struct temp_burst;
	char *aux = (char*) malloc (len+1);
	char* aux_mem;
	
	memset(temp_burst.data, 0x0, EV_BUFSIZE);
	
	if ( aux == NULL ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneBurst MALLOC ERROR");
		return;
		exit(1);
	}	
	
	memset(aux, '\0', len+1);
	memcpy(aux, data, len);
	
	// <length>,<source address>,<destination address>,<bitrate>,<rssi>,
	// <integrity>,<propagation time>,<velocity>,<data>
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneBurst %s len %d - lenAll %d", data, strlen(data), len);
	
	token = strtok_r(data, ",", &aux_mem);
	temp_burst.length = atoi(token);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneBurst temp_burst.length %d", temp_burst.length);
	
	token = strtok_r(NULL, ",", &aux_mem);
	temp_burst.source = atoi(token);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneBurst temp_burst.source %d", temp_burst.source);
	
	token = strtok_r(NULL, ",", &aux_mem);
	temp_burst.destination = atoi(token);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneBurst temp_burst.destination %d", temp_burst.destination);
	
	token = strtok_r(NULL, ",", &aux_mem);
	temp_burst.bitrate = atof(token);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneBurst temp_burst.bitrate %f", temp_burst.bitrate);
	
	token = strtok_r(NULL, ",", &aux_mem);
	temp_burst.rms = atof(token);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneBurst temp_burst.rms %f", temp_burst.rms);
	
	token = strtok_r(NULL, ",", &aux_mem);
	temp_burst.integrity = atof(token);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneBurst temp_burst.integrity %f", temp_burst.integrity);
	
	token = strtok_r(NULL, ",", &aux_mem);
	temp_burst.prop_time = atof(token);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneBurst temp_burst.prop_time %f", temp_burst.prop_time);
	
	token = strtok_r(NULL, ",", &aux_mem);
	temp_burst.velocity = atof(token);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneBurst temp_burst.velocity %f", temp_burst.velocity);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneBurst beforeData len %d temp_im.length %d", len, temp_burst.length);
	
	memcpy(temp_burst.data, (aux + len - temp_burst.length), temp_burst.length);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneBurst temp_burst.data %s", temp_burst.data);
	
	for (int i = 0; i < temp_burst.length; i++) {
		Sunset_Debug::debugInfo(4, getModuleAddress(), "dat(%d) = %x", i, temp_burst.data[i]);
	}
	
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Evologics_v1_4::rxDoneBurst Received EV_RECV,%d,%d,%d,%f,%f,%f,%f,%f,%s ", temp_burst.length, temp_burst.source, temp_burst.destination, temp_burst.bitrate, temp_burst.rms, temp_burst.integrity, temp_burst.prop_time, temp_burst.velocity, temp_burst.data);
	
	pktReceived(temp_burst.source, temp_burst.destination, temp_burst.data, temp_burst.length);
	
	free(aux);	
	
	return;
}

/*!
 * 	@brief The getState() function returns the driver logic state.
 */

int Sunset_Evologics_v1_4::getState()
{
	
	return state;
}

/*!
 * 	@brief The setState() function sets the driver logic state.
 */

void Sunset_Evologics_v1_4::setState(int s) 
{
	state = s;
}

/*!
 * 	@brief The modemTimeout() function is invoked when the modem timeout expires, it means some operation has not been completed in time.
 */

void Sunset_Evologics_v1_4::modemTimeout() 
{
	Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::modemTimeout()");
	
	if ( getState() == EV_SETTING ) {
		
		if ( modem_setting() == true ) {
			
			setState(EV_IDLE);
		}
		
		return;
	}
	
	if ( getState() == EV_RANGING ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::modemTimeout() RANGING");
		
		if (rangingTime == 0.0) {
			
			range_dest = -1;
			setState(EV_IDLE);
			
		} else {
			
			if (rangingTimer_.busy()) {
				
				rangingTimer_.stop();
			}
			
			if (rangingTime > 0.0 && range_dest != -1 ) {
				
				Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::modemTimeout RANGING startTimer");
				
				setState(EV_IDLE);
				
				rangingTimer_.start(rangingTime, range_dest);
			}
			else {
				
				range_dest = -1;
				setState(EV_IDLE);			
			}	      
		}
		
		return;
	}
	
	setState(EV_IDLE);
	
	txAborted();
}

void Sunset_Evologics_v1_4::rttTimeout() 
{
	Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::rttTimeout()");
	
	if (sendRTT_command() == false) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::rttTimeout() ERROR");
		
		if (rangingTime == 0.0) {
			
			range_dest = -1;
			setState(EV_IDLE);
			
		} else {
			
			if (rangingTimer_.busy()) {
				
				rangingTimer_.stop();
			}
			
			if (rangingTime > 0.0 && range_dest != -1 ) {
				
				Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Evologics_v1_4::rttTimeout RANGING startTimer");
				
				setState(EV_IDLE);
				
				rangingTimer_.start(rangingTime, range_dest);
			}
			else {
				
				range_dest = -1;
				setState(EV_IDLE);			
			}
		}
	}
}


void Sunset_Evologics_RangingReply_Timer::start(double time, int dst) 
{
	Scheduler& s = Scheduler::instance();
	assert(busy_ == 0);
	
	if (busy()) {
		
		Sunset_Debug::debugInfo(-1, modem->getModuleAddress(), "Sunset_Evologics_RangingReply_Timer::start busy ERROR");
		return;
	} 
	
	busy_ = 1;
	paused_ = 0;
	s.sync();
	stime = s.clock();
	rtime = time;
	assert(rtime >= 0.0);
	p_ = 0;
	
	Sunset_Utilities::schedule(this, &intr, rtime);
	dest = dst;
}

void Sunset_Evologics_RangingReply_Timer::stop(void) 
{
	Scheduler& s = Scheduler::instance();
	
	assert(busy_);
	
	if(paused_ == 0)
		s.cancel(&intr);
	
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;
	p_ = 0;
	dest = -1;
}

void Sunset_Evologics_RangingReply_Timer::handle(Event *) 
{       
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;
	((Sunset_Evologics_v1_4*)modem)->send_data_imRTT();
}

void Sunset_Evologics_Timeout_Timer::handle(Event *) 
{       
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;
	((Sunset_Evologics_v1_4*)modem)->modemTimeout();
}

void Sunset_Evologics_RTT_Timer::handle(Event *) 
{       
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;
	((Sunset_Evologics_v1_4*)modem)->rttTimeout();
}


void Sunset_Evologics_DeliveryStatus_Timer::handle(Event *) 
{       
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;
	((Sunset_Evologics_v1_4*)modem)->modemDeliveryTimeout();
}


void Sunset_Evologics_v1_4::modemBurstMsgRespTimeout() 
{
	Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::modemBurstMsgRespTimeout() Data burst: No response from modem!");
	setState(EV_IDLE);
	
	txAborted();
}

void Sunset_Evologics_BurstMsgResp_Timer::handle(Event *) 
{       
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;
	((Sunset_Evologics_v1_4*)modem)->modemBurstMsgRespTimeout();
}

void Sunset_Evologics_Start_Connection::handle(Event *) 
{       
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;
	((Sunset_Evologics_v1_4*)modem)->handleConnection();
}

void Sunset_Evologics_Rx_Timer::handle(Event *) 
{       
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;
	((Sunset_Evologics_v1_4*)modem)->processRxInfo();
        
	if (!(((Sunset_Evologics_v1_4*)modem)->rx_info).empty()) {

		handle((Event*)0);
		return;
	} 
}

void Sunset_Evologics_v1_4::handleConnection() 
{
	
	Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::handleConnection retry");
	
	setState(EV_IDLE);
	resetTx();
	listening = 0;
	
	startConnection();
}

/*!
 * 	@brief The modemDeliveryTimeout() function is invoked when the modem packet delivery timeout expires, it means that data transmission has not been completed in time.
 */

void Sunset_Evologics_v1_4::modemDeliveryTimeout() 
{
	
	if ( getState() == EV_WAIT_DATA_STATUS ) {
		
		deliveryRetry++;
		
		if ( deliveryRetry >= EV_TIMEOUT_DLV_MAX_RET ) {
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::modemDeliveryTimeout() Max pkt status retry (%d) reached -> ABORTED ", deliveryRetry);
		
			setState(EV_IDLE);
			
			txAborted();
			deliveryRetry = 0;
			
			return;
			
		} 
		
		if ( checkDeliveryStatus() == false ) {
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Evologics_v1_4::modemDeliveryTimeout() check_delivery ");
			
			setState(EV_IDLE);
			
			txAborted();
			deliveryRetry = 0;
			
			return;
		}
		
		Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Evologics_v1_4::modemDeliveryTimeout() -> Retry (%d)", deliveryRetry);
		
		if (timeoutDeliv_.busy()) {
			
			timeoutDeliv_.stop();
		}
		
		if (deliveryRetry == 0 && tx_time > 0.0) {
			
			Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Evologics_v1_4::modemDeliveryTimeout() delay tx time %f deliveryRetry %d", tx_time, deliveryRetry);
			
			timeoutDeliv_.start(tx_time);
		} 
		else {
			
			Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Evologics_v1_4::modemDeliveryTimeout() delay %f deliveryRetry %d", EV_TIMEOUT_DLV, deliveryRetry);
			
			timeoutDeliv_.start(EV_TIMEOUT_DLV);
		}
	}
}

int Sunset_Evologics_v1_4::notify_info(list<notified_info> linfo) 
{ 
	list<notified_info>::iterator it = linfo.begin();
	notified_info ni;
	string s;
	char msg[300];
	memset(msg, '\0', 300);
	double val_double = 0.0;
	int val_int = 0;
	node_position aux;
	
	for (; it != linfo.end(); it++) {
		
		ni = *it;
		
		s = "TX_POWER";
		
		if (strncmp((ni.info_name).c_str(), s.c_str(), strlen(s.c_str())) == 0 ) {
			
			sid->get_value(&val_int, ni);
			
			if (ni.node_id == getModuleAddress() && current_tx_power != val_int) {
		
				struct evo_setting cmd;
				
				if (getState() != EV_IDLE) {
				
					Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_Evologics_v1_4::notify_info OVERRIDE STATE to write tx power %d", val_int);
				}
				
				setState(EV_SETTING);
				
				cmd.type = EV_SET_POWER;
				cmd.value = val_int;
				
				setting_cmd.push(cmd);
				
				modem_setting();
				
			}
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::notify_info tx power %d", val_int);
			
			return 1;
		}
		
		s = "MAC_SLOT_COUNT";
		
		if (strncmp((ni.info_name).c_str(), s.c_str(), strlen(s.c_str())) == 0 ) {
			
			sid->get_value(&val_int, ni);			
			
			mac_slot = val_int;
			
			Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_Evologics_v1_4::notify_info mac slot %d", val_int);
			
			return 1;
		}
	}
	
	Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_Evologics_v1_4::notify_info NOTHING TO DO");
	
	return Sunset_Generic_Modem::notify_info(linfo); 
} 

int Sunset_Evologics_v1_4::setTxPower(int node, int tx_level, double time) 
{
	
	if (sid == NULL) {
	
		return 0;
	}
	
	notified_info ni;
	
	ni.node_id = node;
	ni.info_time = time;
	ni.info_name = "TX_POWER";
	
	if ( sid->assign_value(&tx_level, &ni, sizeof(int)) == false) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::setTxPower ERROR ASSIGINING INFO %s", (ni.info_name).c_str());
		
		return 0;	
	}
	
	if ( sid->set(getModuleAddress(), sid_id, ni) == 0 ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Evologics_v1_4::setTxPower PROVIDING INFO %s NOT DEFINED", (ni.info_name).c_str());
		
		return 0;
	}
	
	Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Evologics_v1_4::setTxPower node %d val %d", (node), tx_level);
	
	return 1;
}
