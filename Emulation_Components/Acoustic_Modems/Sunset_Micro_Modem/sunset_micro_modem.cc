/* SUNSET - Sapienza University Networking framework for underwater Simulation, Emulation and real-life Testing
 *
 * Copyright (C) 2012 Regents of UWSN Group of SENSES Lab <http://reti.dsi.uniroma1.it/SENSES_lab/>
 *
 * Author: Roberto Petroccia - petroccia@di.uniroma1.it
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


#include <sunset_debug.h>
#include <sunset_statistics.h>

#include <sunset_micro_modem.h>

extern "C" void *ThreadStartupMicroModem_listen(void *);
pthread_mutex_t mutex_mm_timer_rx;
pthread_mutex_t mutex_mm_timer_connection;

void Sunset_MM_TimeoutTimer::handle(Event *) 
{       
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;
	((Sunset_MicroModem*)modem)->modemTimeout();
}

void Sunset_MM_SetUpTimer::handle(Event *) 
{       
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;
	((Sunset_MicroModem*)modem)->sendSetUpCmd();
}

void Sunset_MM_RecvTimer::handle(Event *) 
{       
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;
	((Sunset_MicroModem*)modem)->recvBufferData();
}

void Sunset_MM_ReconnTimer::handle(Event *) 
{       
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;
	((Sunset_MicroModem*)modem)->connect();
}

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_MicroModemClass : public TclClass 
{
public:
	
	Sunset_MicroModemClass() : TclClass("Sunset_MicroModem") {}
	
	TclObject* create(int argc, const char*const* argv) {
		
		return (new Sunset_MicroModem());
	}
	
} class_Sunset_MicroModem;

Sunset_MicroModem::Sunset_MicroModem() : Sunset_Generic_Modem(), timeoutTimer_(this), setupTimer_(this), recvTimer_(this), reconnTimer_(this)
{
	modem_checkSum = 0;
	USE_ACK = 0;
	receivedPkt = 0;
	
	use_XST = 0;
	use_CST = 0;
	want_ACK = 0;
	mm_conn = 0;
	ERROR_VAL = -1;
	ERROR_MSG = NULL;
	MSG_VAL = -1;
	MSG_TYPE = NULL;
	listening = 0;
	mmControl = 1;
	
	bind("AGN_VALUE", &AGN_VALUE);
	bind("CTO_VALUE", &CTO_VALUE);
	bind("MODEM_RATE_", &MODEM_RATE);
	bind("USE_ASCII_", &USE_ASCII);
	bind("USE_HEX_", &USE_HEX);
	bind("USE_ACK_", &USE_ACK);
	bind("USE_GPS_INFO_", &USE_GPS_INFO);
	bind("MINI_PKT_", &useMiniPkt);
	
	if (USE_ASCII == USE_HEX) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem set up USE_HEX = USE_ASCII ERROR");
		
		return;   //TODO: it's just to avoid exiting the simulation
		exit(1);
	}
	
	if (MODEM_RATE > 6 || MODEM_RATE < 0) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem set up MODEM RATE %d ERROR", MODEM_RATE);
		
		return;   //TODO: it's just to avoid exiting the simulation
		exit(1);
	}
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem use ack %d miniPkt %d rate %d hex %d ascii %d agn %d", USE_ACK, useMiniPkt, MODEM_RATE, USE_HEX, USE_ASCII, AGN_VALUE);
	
	STATE = MM_S_WAIT_CMD;
	d_status = MM_DRIVER_IDLE;
	setupRetry = 0;
	
	mm_messages = new MicroModem_Messages();
	
	pthread_mutex_init(&mutex_mm_timer_rx, NULL);
	pthread_mutex_init(&mutex_mm_timer_connection, NULL);
}

Sunset_MicroModem::~Sunset_MicroModem() 
{
	listening = 0;
	
	pthread_mutex_destroy(&mutex_mm_timer_rx);
	pthread_mutex_destroy(&mutex_mm_timer_connection);
	
	disconnect(mm_conn->get_fd());
}

/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script 
 *	@param argc argc is a count of the arguments supplied to the command function.
 *	@param argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_MicroModem::command( int argc, const char * const * argv ) 
{
	if ( argc == 3 ) {
		
		if (strcmp(argv[1], "checkSum") == 0) {
			
			modem_checkSum = atoi(argv[2]);
			
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::command use checkSum %ld", modem_checkSum);
			
			return TCL_OK;
		}
		
		if (strcmp(argv[1], "mmControl") == 0) {
			
			mmControl = atoi(argv[2]);
			
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::command use mmControl %d", mmControl);
			
			return TCL_OK;
		}
	}
	
	return Sunset_Generic_Modem::command( argc, argv );
}

void *ThreadStartupMicroModem_listen(void *_tgtObject) 
{
	
	pthread_detach(pthread_self());
	
	Sunset_MicroModem *tgtObject;
	
	tgtObject = (Sunset_MicroModem*)_tgtObject;
	tgtObject->rx_thread();
	
	return (NULL);
}

/*!
 * 	@brief The disconnect() function closes a serial line connection.
 * 	@retval 0 in case of error, 1  otherwise.
 */

int Sunset_MicroModem::disconnect(int fd) 
{
	mm_conn->close_connection();
}

/*!
 * 	@brief The connect() function opens a serial line connection. 
 *	It also starts the thread responsible for reading messages from the modem on the serial line
 * 	@retval 0 in case of error, the file descriptor fo the serial line connection otherwise.
 */

int Sunset_MicroModem::connect() 
{
	
	pthread_t readerThreadId;
	int rc = 0;
	int x = 0;
	listening = 0;
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::connect connecting to %s baudRate %ld", devName, baudRate);
	
	if (!devName || baudRate == 0) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::connect devName has not been defined ERROR");
		
		exit(1);
	}
	
	mm_conn = new Sunset_MicroModem_Conn(devName, baudRate);
	
	if (!(mm_conn->open_connection())) {
		
		listening = 0;
		
		pthread_mutex_lock(&mutex_mm_timer_connection);
		
		if (reconnTimer_.busy()) {
			
			reconnTimer_.stop();
		}
		
		reconnTimer_.start(MM_TIMEOUT_CONNECTION);
		
		pthread_mutex_unlock(&mutex_mm_timer_connection);
		
		return 0;
	}
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::connect connection done");

	listening = 1;
	
	rc = pthread_create(&readerThreadId, NULL, ThreadStartupMicroModem_listen, (void *)this);
	
	if ( rc != 0 ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::connect initialize reading thread ERROR on %s baud %d", mm_conn->getDevName(), mm_conn->getIntBaudRate());
		
		exit(1);
	}
	else {
		Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::connect initialize reading thread to %s baud %ld DONE", mm_conn->getDevName(), mm_conn->getIntBaudRate());
	}
	
	if ( mmControl == 1 ) {
		
		settingModem(USE_HEX, USE_ASCII, MM_MODEM_PORT);
	}
	
	x = mm_conn->get_fd();
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::connect connecting fd = %d", x);
	
	return x;
}


/*!
 * 	@brief The addSetUpCmd() function adds a new message setting modem parameters the the list of setting commands.
 *	It is done when the emulation starts and the modem parameters have to be properly set according to the information
 *	provided by the user in the TCL script.
 * 	@retval The file descriptor of the opened connection.
 */

int Sunset_MicroModem::addSetUpCmd(char* cmd, int req) 
{
	int len = 0;
	char* msg;
	pair<int, char*> pr;
	
	if (strlen(cmd) == 0) {
		
		return 0;
	}
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::addSetUpCmd %s - %d", cmd, req);
	
	len = strlen(cmd) + 1;	
	msg = (char*) malloc(len * sizeof(char));
	
	if ( msg == NULL ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::addSetUpCmd MALLOC ERROR");
		exit(1);
	}
	
	memset(msg, '\0', len);
	memcpy(msg, cmd, len-1);
	
	pr.first = req;
	pr.second = msg;
	
	setupInfo.push_back(pr);
	
	return 1;
}

/*!
 * 	@brief The getSetUpInfo() function extracts the parameter ID and value from a given buffer containing a setting message
 *	@param buf The buffer containing the setting message.
 *	@param[out] key The parameter ID.
 *	@param[out] val The value to be assigned to the parameter.
 * 	@retval 0 in case of error, 1  otherwise.
 */

int Sunset_MicroModem::getSetUpInfo(char* buf, char* key, int* val) 
{
	char *pch;
	char *aux_mem;
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::getSetUpInfo %s", buf);
	
	pch = strtok_r (buf,",", &aux_mem);
	
	if (pch == NULL) {
		
		return 0;
	}
	
	pch = strtok_r (NULL,",", &aux_mem);
	
	if (pch == NULL) {
		
		return 0;
	}
	
	memcpy(key, pch, strlen(pch));
	
	pch = strtok_r (NULL,",", &aux_mem);
	
	if (pch == NULL) {
		
		return 0;
	}
	
	*val = atoi(pch);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::getSetUpInfo %s %d", key, *val);
	
	return 1;
} 

/*!
 * 	@brief The checkSetUpCmd() function checks if a parameter has been correctly set and initializes internal values of the Micro-Modem driver.
 *	@param buf The buffer containing the setting message.
 * 	@retval 0 in case of error, 1  otherwise.
 */

int Sunset_MicroModem::checkSetUpCmd(char* buf) 
{
	int pktType = 0;
	cacfg_t cafg;
	pair<int, char*> pr;
	char cmd[UMMAXMSSZ];
	char key[UMMAXMSSZ] = {'\0'};
	int val = -1;
	
	if (setupInfo.empty()) {
		
		return 0;
	}
	
	pktType = mm_messages->NMEAPktType(buf);
	
	if (mm_messages->sscannmea(buf, pktType, &cafg) <= 0) {
		
		return 0;	
	}
	pr = setupInfo.front();
	
	memset(cmd, '\0', UMMAXMSSZ);
	memcpy(cmd, buf, strlen(buf));
	
	if (!getSetUpInfo(cmd, key, &val)) {
		
		return 0;
	}
	
	/* check cmd and value */
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::checkSetUpCmd %s - key %s val %d len %d - my key %s my val %d may len %d", buf, cafg.key, cafg.val, strlen(cafg.key), key, val, strlen(key));
	
	if (strlen(cafg.key) == strlen(key) && 
	    strncmp(cafg.key, key, strlen(cafg.key)) == 0 && cafg.val == val) {
		
		if (strncmp(cafg.key, "XST", strlen(cafg.key)) == 0 &&
		    cafg.val == 1) {
			
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::checkSetUpCmd set XST");
			
			use_XST = 1;
		}
		
		if (strncmp(cafg.key, "CST", strlen(cafg.key)) == 0 &&
		    cafg.val == 1) {
			
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::checkSetUpCmd setCST");
			
			use_CST = 1;
		}
		
		return 1;
	}
	
	return 0;
}

/*!
 * 	@brief The setUpCmdDone() function is called when a setting message has been executed.
 *	If other setting commands wait to be executed, this function starts the execution of the next setting command in the list.
 *	@param buf The buffer containing the executed setting message.
 * 	@retval 0 in case of error, 1  otherwise.
 */

int Sunset_MicroModem::setUpCmdDone(char* buf) 
{
	pair<int, char*> pr;
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::setUpCmdDone %s", buf);
	
	if (!checkSetUpCmd(buf)) {
		
		Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::setUpCmdDone CHECK exit");
		return 0;
	}	
	
	if (timeoutTimer_.busy()) {
		
		timeoutTimer_.stop();
	}
	
	pr = setupInfo.front();
	
	free(pr.second);
	
	setupInfo.pop_front();	
	
	setupRetry = 0;	
	
	if (setupInfo.empty()) {
		
		if (d_status == MM_DRIVER_SETUP) {
			
			d_status = MM_DRIVER_IDLE;
		}
		
		Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::setUpCmdDone FINISH");
		
		return 1;
	}
	else {
		
		setupTimer_.start(0.001);
	}
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::setUpCmdDone DONE move NEXT");
	
	return 1;
}

/*!
 * 	@brief The checkConnection() function checks if the serial line connection has been created and started.
 * 	@retval 0 in case of error, 1  otherwise.
 */

int Sunset_MicroModem::checkConnection() 
{
	if (mm_conn == 0 || mm_conn->get_fd() <= 0) {
		
		return 0;
	}
	
	return 1;
}

/*!
 * 	@brief The sendSetUpCmd() function sends a setting message to the modem.
 * 	@retval -1 in case of error, 1  if there are no more setting commands, 0 otherwise.
 */

int Sunset_MicroModem::sendSetUpCmd() 
{
	int ww = 0;
	pair<int, char*> pr;
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::sendSetUpCmd %d", setupRetry);
	
	if (setupRetry > MAX_SETUP_RETRY && !(setupInfo.empty())) {
		
		pair<int, char*> pr;
		
		setupRetry = 0;
		
		pr = setupInfo.front();
		
		if (pr.first == 0) {
			
			free(pr.second);
			setupInfo.pop_front();			
		}
	}
	
	if (setupInfo.empty()) {
		
		if (d_status == MM_DRIVER_SETUP) {
			
			d_status = MM_DRIVER_IDLE;
		}
		
		return 1;
	}
	
	setupRetry++;
	
	pr = setupInfo.front();
	
	if (writeDataToModem(pr.second, strlen(pr.second), TIMEOUT_MM_SETUP) == 1) {
		
		return 0;
	}
	
	return -1;
}


/*!
 * 	@brief The settingModem() function creates all the setting messages and adds them to the setting list.
 * 	@retval -1 in case of error, 1 otherwise.
 */

int Sunset_MicroModem::settingModem(int hex, int ascii, int port)
{
	char buf[UMMAXMSSZ] = {'\0'};
	char bRateString[4] = {'\0'};
	
	memset(buf, '\0', sizeof(char) * UMMAXMSSZ);
	mm_messages->creaPktCCCFG(buf, UMMAXMSSZ, (char*)"CTO", CTO_VALUE, 0);	//Turn on transmit stats message, CAXST
	
	if (!addSetUpCmd(buf, 0)) {
		
		return -1;
	}
	
	memset(buf, '\0', sizeof(char) * UMMAXMSSZ);
	mm_messages->creaPktCCCFG(buf, UMMAXMSSZ, (char*)"AGN", AGN_VALUE, 0);	//Turn on transmit stats message, CAXST
	
	if (!addSetUpCmd(buf, 0)) {
		
		return -1;
	}
	
	memset(buf, '\0', sizeof(char) * UMMAXMSSZ);
	mm_messages->creaPktCCCFG(buf, UMMAXMSSZ, (char*)"XST", 0, 0);	//Turn on transmit stats message, CAXST
	
	if (!addSetUpCmd(buf, 0)) {
		
		return -1;
	}
	
	memset(buf, '\0', sizeof(char) * UMMAXMSSZ);
	mm_messages->creaPktCCCFG(buf, UMMAXMSSZ, (char*)"DQF", 1, 0);	// Cycle statistics message
	
	if (!addSetUpCmd(buf, 0)) {
		
		return -1;
	}	
	
	memset(buf, '\0', sizeof(char) * UMMAXMSSZ);
	mm_messages->creaPktCCCFG(buf, UMMAXMSSZ, (char*)"SRC", getModuleAddress(), 0);	// Cycle statistics message
	
	if (!addSetUpCmd(buf, 1)) {
		
		return -1;
	}
	
	memset(buf, '\0', sizeof(char) * UMMAXMSSZ);
	mm_messages->creaPktCCCFG(buf, UMMAXMSSZ, (char*)"CST", 1, 0);	// Cycle statistics message
	
	if (!addSetUpCmd(buf, 0)) {
		
		return -1;
	}
	
	memset(buf, '\0', sizeof(char) * UMMAXMSSZ);
	mm_messages->creaPktCCCFG(buf, UMMAXMSSZ, (char*)"RXP", 1, 0); //Whether or not to send the $CARXPmessage
	
	if (!addSetUpCmd(buf, 0)) {
		
		return -1;
	}
	
	memset(buf, '\0', sizeof(char) * UMMAXMSSZ);
	mm_messages->creaPktCCCFG(buf, UMMAXMSSZ, (char*)"RXD", hex, 0); // Whether or not to send the $CARXD message
	
	if (!addSetUpCmd(buf, 0)) {
		
		return -1;
	}
	
	memset(buf, '\0', sizeof(char) * UMMAXMSSZ);
	mm_messages->creaPktCCCFG(buf, UMMAXMSSZ, (char*)"RXA", ascii, 0); // Whether or not to send the $CARXA message
	
	if (!addSetUpCmd(buf, 0)) {
		
		return -1;
	}
	
	memset(buf, '\0', sizeof(char) * UMMAXMSSZ);
	mm_messages->creaPktCCCFG(buf, UMMAXMSSZ, (char*)"REV", 1, 0); // Whether or not to send the $CAREV message
	
	if (!addSetUpCmd(buf, 0)) {
		
		return -1;
	}
	
	memset(buf, '\0', sizeof(char) * UMMAXMSSZ);
	mm_messages->creaPktCCCFG(buf, UMMAXMSSZ, (char*)"MFD", 0, 0); // Whether or not to send the $CAMFD message
	
	if (!addSetUpCmd(buf, 0)) {
		
		return -1;
	}
	
	memset(buf, '\0', sizeof(char) * UMMAXMSSZ);
	mm_messages->creaPktCCCFG(buf, UMMAXMSSZ, (char*)"SHF", 0, 0); // Whether or not to send the $CASHF message
	
	if (!addSetUpCmd(buf, 0)) {
		
		return -1;
	}
	
	memset(buf, '\0', sizeof(char) * UMMAXMSSZ);
	
	if(port == 1) {
		
		sprintf(bRateString, "%s", "BR1");
	}
	else {
		
		sprintf(bRateString, "%s", "BR2");
	}
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::settingModem before setting baudRate");	
	
	switch(baudRate) {
			
		case 2400:
		{
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::settingModem 2400");	
			mm_messages->creaPktCCCFG(buf, UMMAXMSSZ, bRateString, 0, 0);
			
			break;
		}
			
		case 4800:
		{
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::settingModem 4800");	
			mm_messages->creaPktCCCFG(buf, UMMAXMSSZ, bRateString, 1, 0);
			
			break;
		}
			
		case 9600:
		{
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::settingModem 9600");	
			mm_messages->creaPktCCCFG(buf, UMMAXMSSZ, bRateString, 2, 0);
			
			break;
		}
			
		case 19200:
		{
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::settingModem 19200");	
			mm_messages->creaPktCCCFG(buf, UMMAXMSSZ, bRateString, 3, 0);
			
			break;
		}
			
		case 38400:
		{
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::settingModem 38400");	
			mm_messages->creaPktCCCFG(buf, UMMAXMSSZ, bRateString, 4, 1);
			
			break;
		}
			
		case 57600:
		{
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::settingModem 57600");	
			mm_messages->creaPktCCCFG(buf, UMMAXMSSZ, bRateString, 5, 0);
			
			break;
		}
			
		case 115200:
		{
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::settingModem 115200");	
			mm_messages->creaPktCCCFG(buf, UMMAXMSSZ, bRateString, 6, 0);
			
			break;
		}
			
		default:
		{
			/* unsupported speed: default value */
			
			mm_messages->creaPktCCCFG(buf, UMMAXMSSZ, bRateString, 6, 0);
			break;
		}
	}
	
	if (!addSetUpCmd(buf, 0)) {
		
		return -1;
	}
	
	d_status = MM_DRIVER_SETUP;
	
	sendSetUpCmd();	
	
	return 1;
}

/*!
 * 	@brief The isTxStatus() function checks it the Micro-Modem driver status in input is a transmission status, i.e. a status indicating involvement in a data transmission.
 *	@param status The status to check
 * 	@retval 1 if it is a transmission status, 0 otherwise.
 */

int Sunset_MicroModem::isTxStatus(mm_driver_status status) 
{
	if (status == MM_DRIVER_TX_CYC || status == MM_DRIVER_TX_MINI_PKT || 
		status == MM_DRIVER_TX_DATA  || status == MM_DRIVER_WAIT_DRQ || 
		status == MM_DRIVER_WAIT_MINI_PKT_XST || status == MM_DRIVER_WAIT_DATA_XST || 
		status == MM_DRIVER_WAIT_ACK) {
		
		return 1;
	}
	
	return 0;
}

/*!
 * 	@brief The isRxStatus() function checks it the Micro-Modem driver status in input is a reception status, i.e. a status indicating involvement in a data reception.
 *	@param status The status to check
 * 	@retval 1 if it is a reception status, 0 otherwise.
 */

int Sunset_MicroModem::isRxStatus(mm_driver_status status) 
{
	if (status == MM_DRIVER_RX_DATA || status == MM_DRIVER_RX_MINI_PKT ||
		status == MM_DRIVER_WAIT_MINI_PKT_CST || 
		status == MM_DRIVER_WAIT_DATA_CST || status == MM_DRIVER_RX_CYC) {
		
		return 1;
	}
	
	return 0;
}

/*!
 * 	@brief The modemTimeout() function is called when a timeut on some modem operation occurred. It ususally indicates a not completed operation.
 */

void Sunset_MicroModem::modemTimeout() 
{
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::modemTimeout d_status %d", d_status);
	
	if (d_status == MM_DRIVER_SETUP) {
		
		setupRetry++;
		sendSetUpCmd();
		
		return;
	}
	else if (isTxStatus(d_status)) {
		
		txAborted();
		d_status = MM_DRIVER_ERROR;
		
		return;
	}
	else if (isRxStatus(d_status)) {
		
		rxAborted();
		d_status = MM_DRIVER_ERROR;
		
		return;
	}
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::modemTimeout d_status %d ERROR", d_status);
	
	exit(1);
	return;
}

/*!
 * 	@brief The pktWrite function returns the buffer to be transmitted to the Micro-Modem, corresponding to the ns-2 packet in input after the conversion and the encoding operations.
 *	@param[in] p The packet to be converted.
 *	@param[out] len The length of the bytes array to be transmitted.
 * 	@retval The  bytes array to be transmitted.
 */

char* Sunset_MicroModem::pktWrite(Packet* p, int& len) 
{
	int dataLen = 0;
	char* bufferData;
	
	bufferData = pkt2Modem(p, dataLen);
	
	if( bufferData == NULL ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::pktWrite no bufferData ERROR");
		return NULL;
	}
	
	len = dataLen;
	
	return bufferData;
}

/*!
 * 	@brief The sendMiniPkt function transmits the ns-2 packet in input as a mini-packet after the conversion and the encoding operations.
 *	@param[in] p The packet to be sent.
 */

void Sunset_MicroModem::sendMiniPkt(Packet *p) 
{
	char* buffer;
	int len = 0;
	int dst, src;	
	
	pktConverter_->getSrc(p, UW_PKT_MAC, src);
	pktConverter_->getDst(p, UW_PKT_MAC, dst);
	
	buffer = createDownCtrlPkt(p, len);
	
	if (Sunset_Statistics::use_stat() && stat != NULL) {
		
		stat->logStatInfo(SUNSET_STAT_MODEM_GET_PKT, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");	
	}
	
	Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_MicroModem::sendMiniPkt write miniPkt buff %d len %d", (char*)buffer, len);
	
	if (!writeMiniPkt(buffer, len, dst)) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::sendMiniPkt cannot write data over the serial port ERROR");
	}
	
	free(buffer);	
}

/*!
 * 	@brief The sendDataPkt function transmits the ns-2 packet in input as to the modem, after the conversion and the encoding operations.
 *	@param[in] p The packet to be sent.
 */

void Sunset_MicroModem::sendDataPkt(Packet *p) 
{
	char* buffer;
	int len = 0;
	int dst = 0, src = 0;	
	
	pktConverter_->getSrc(p, UW_PKT_MAC, src);
	pktConverter_->getDst(p, UW_PKT_MAC, dst);
	
	buffer = pktWrite(p, len);
	
	if (buffer == NULL) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::sendDataPkt cannot write data over the serial port ERROR");
		
		exit(1);
	}
	
	if (Sunset_Statistics::use_stat() && stat != NULL) {
		
		stat->logStatInfo(SUNSET_STAT_MODEM_GET_PKT, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");
		
	}
	
	if (!writeModem(buffer, len, dst)) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::sendDataPkt data buffer ERROR");
	}
	
	free(buffer);
}

/*!
 * 	@brief The sendDown function converts the ns-2 packet into a stream of bytes and sends such stream to the Micro-Modem.
 *	@param p The packet to be sent.
 */

void Sunset_MicroModem::sendDown(Packet *p) 
{
	int dst = 0, src = 0;	
	
	pktConverter_->getSrc(p, UW_PKT_MAC, src);
	pktConverter_->getDst(p, UW_PKT_MAC, dst);
	
	if (src != getModuleAddress()) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Modem recv sending DOWN packet with src %d ERROR", src);
		
		exit(1);
	}
	
	if (d_status == MM_DRIVER_SETUP) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::sendDown - SETUP not yet completed ERROR");
		
		pktTxList.push_back(p);
		txAborted();
		
		return;
	}
	
	if (isRxStatus(d_status)) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::sendDown - modem is receiving  ERROR");
		
		if (Sunset_Statistics::use_stat() && stat != NULL) {
			
			stat->logStatInfo(SUNSET_STAT_MODEM_TX_ABORTED, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");	
			
		}
		
		Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::sendDown txAborted DATA tx %d", (int)(pktTxList.size()));
		
		Modem2PhyTxAborted(p);
		
		return;
	}
	
	if (d_status == MM_DRIVER_ERROR) {
		
		d_status = MM_DRIVER_IDLE;
	}
	
	if (d_status != MM_DRIVER_IDLE || !listCycToSend.empty() || !listPktToSend.empty() || !pktTxList.empty()){
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::sendDown - state is not idle %d ERROR", STATE);
		
		exit(1);
	}
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::sendDown pkt");
	
	pktTxList.push_back(p);
	
	if (useMiniPkt) {
		
		int result = 0;
		
		result = pktConverter_->useMiniPkt(p, UW_PKT_MAC);
		
		if (result == 1) {
			sendMiniPkt(p);		
			return;		
		}
	}
	
	sendDataPkt(p);
}


/*!
 * 	@brief The writeDataToModem function sends a stream of bytes to the Micro-Modem and starts waiting a given amount of seconds before notifying a timeout in the current operation.
 *	@param buf The buffer to be sent.
 *	@param len The  length of the buffer.
 *	@param timeout The time allowed to complete this operation.
 * 	@retval -1 in case of error, 1 otherwise.
 */

int Sunset_MicroModem::writeDataToModem(const char *  buf, const size_t len, double timeout) 
{
	Packet* p;
	double realtime;
	int ww;
	
	if (!checkConnection()) {
		
		return -1;
	}
	
	p = getPktTxList();
	
	if (p == 0) {
		
		if (Sunset_Statistics::use_stat() && stat != NULL) {
			
			stat->logStatInfo(SUNSET_STAT_MODEM_INFO, getModuleAddress(), 0, 0, "%s", buf);	
			
		}
	}
	else {
		
		if (Sunset_Statistics::use_stat() && stat != NULL) {
			
			stat->logStatInfo(SUNSET_STAT_MODEM_INFO, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "%s", buf);	
		}
	}
	
	realtime = Sunset_Utilities::getRealTime();
	
	ww = mm_conn->write_data((char *)buf, len);
	
	if(ww < 0)  {
		
		return -1;
	}
	
	if (timeoutTimer_.busy()) {
		
		timeoutTimer_.stop();
	}
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::writeTomodem sending: %s - len %d - check %d status %d", buf, len, ww, d_status);
	
	if (d_status == MM_DRIVER_TX_CYC) {
		
		if (p == 0) {
			
			if (Sunset_Statistics::use_stat() && stat != NULL) {
				
				stat->logStatInfo(SUNSET_STAT_MODEM_TRANSFERT_START, getModuleAddress(), 0, 0, "%d\n", len);	
			}
		}
		else {
			
			if (Sunset_Statistics::use_stat() && stat != NULL) {
				
				stat->logStatInfo(SUNSET_STAT_MODEM_TRANSFERT_START, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "%d\n", len);		
			}
		}
	}
	
	if (d_status == MM_DRIVER_TX_DATA) {
		
		if (p == 0) {
			
			if (Sunset_Statistics::use_stat() && stat != NULL) {
				
				stat->logStatInfo(SUNSET_STAT_MODEM_TRANSFERT_END, getModuleAddress(), 0, 0, "%d\n", len);		
			}
		}
		else {
			
			if (Sunset_Statistics::use_stat() && stat != NULL) {
				
				stat->logStatInfo(SUNSET_STAT_MODEM_TRANSFERT_END, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "%d\n", len);	
			}
		}
	}
	
	timeoutTimer_.start(timeout);
	
	return 1;
}

/*!
 * 	@brief The txAborted function is called when a packet transmission has been aborted because of a timeout or an 
 *	error message received from the modem. All the status and temporary data structures are cleared from the information 
 *	regarding the aborted transmission.
 */

void Sunset_MicroModem::txAborted() 
{
	
	if (d_status == MM_DRIVER_SETUP) {
		
		sendSetUpCmd();
		return;
	}
	
	if (d_status == MM_DRIVER_TX_MINI_PKT || d_status == MM_DRIVER_WAIT_MINI_PKT_XST) {
		
		Packet* p;
		
		if (pktTxList.empty()) {
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::txAborted MINI_PKT- Something is wrongtx %d ERROR", (int)(pktTxList.size()));
			
			d_status = MM_DRIVER_IDLE;
			STATE = MM_S_WAIT_CMD;
			
			exit(1);
		}
		
		if (timeoutTimer_.busy()) {
			
			timeoutTimer_.stop();
		}
		
		p = getPktTxList();
		
		if (Sunset_Statistics::use_stat() && stat != NULL) {
			
			stat->logStatInfo(SUNSET_STAT_MODEM_TX_ABORTED, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");		
		}
		
		pktTxList.pop_front();
		
		Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::txAborted MINI_PKT cyc %d send %d tx %d", (int)(listCycToSend.size()), (int)(listPktToSend.size()), (int)(pktTxList.size()));
		
		Modem2PhyTxAborted(p);
		
		d_status = MM_DRIVER_IDLE;
		STATE = MM_S_WAIT_CMD;
		
		return;
	}
	
	if (isTxStatus(d_status)) {
		
		Packet* p;
		
		if (listCycToSend.empty() || listPktToSend.empty() || pktTxList.empty()) {
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::txAborted DATA - Something is wrong with pktList cyc %d send %d tx %d ERROR", (int)(listCycToSend.size()), (int)(listPktToSend.size()), (int)(pktTxList.size()));
			
			d_status = MM_DRIVER_IDLE;
			STATE = MM_S_WAIT_CMD;
			
			exit(1);
		}
		
		if (timeoutTimer_.busy()) {
			
			timeoutTimer_.stop();
		}
		
		free(listCycToSend.front());
		free(listPktToSend.front());
		listCycToSend.pop_front();
		listPktToSend.pop_front();
		
		p = getPktTxList();
		
		if (Sunset_Statistics::use_stat() && stat != NULL) {
			
			stat->logStatInfo(SUNSET_STAT_MODEM_TX_ABORTED, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");	
		}
		
		pktTxList.pop_front();
		
		Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::txAborted2 DATA cyc %d send %d tx %d", (int)(listCycToSend.size()), (int)(listPktToSend.size()), (int)(pktTxList.size()));
		
		Modem2PhyTxAborted(p);
		
		want_ACK = 0;
		d_status = MM_DRIVER_IDLE;
		STATE = MM_S_WAIT_CMD;
		
		return;
	}
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::txAborted NO TX STATUS cyc %d send %d tx %d", (int)(listCycToSend.size()), (int)(listPktToSend.size()), (int)(pktTxList.size()));
	
	if (d_status == MM_DRIVER_ERROR) {
		
		Packet* p = getPktTxList();
		
		if (p != 0) {
			
			if (Sunset_Statistics::use_stat() && stat != NULL) {
				
				stat->logStatInfo(SUNSET_STAT_MODEM_TX_ABORTED, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");	
			}
			
			pktTxList.pop_front();
			Modem2PhyTxAborted(p);
			
			Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::txAborted NO TX STATUS cyc %d send %d tx %d", (int)(listCycToSend.size()), (int)(listPktToSend.size()), (int)(pktTxList.size()));
			
			clearTxInfo();
			
			return;
		}
	}
	
	exit(1);
}

/*!
 * 	@brief The clearTxInfo function is clears all the information regarding the last transmission on-going.
 */

void Sunset_MicroModem::clearTxInfo() 
{
	int check = 0;
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::clearTxInfo start STATE %d d_status %d", STATE, d_status);
	
	if (!listCycToSend.empty()) {
		
		free(listCycToSend.front());
		listCycToSend.pop_front();
		check = 1;
	}
	
	if (!listPktToSend.empty()) {
		
		free(listPktToSend.front());
		listPktToSend.pop_front();
		check = 1;
	}
	
	if (!pktTxList.empty()) {
		
		Packet* p = getPktTxList();
		
		if (Sunset_Statistics::use_stat() && stat != NULL) {
			
			stat->logStatInfo(SUNSET_STAT_MODEM_TX_ABORTED, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");	
		}
		
		pktTxList.pop_front();
		check = 1;
	}
	
	want_ACK = 0;
	
	if (d_status == MM_DRIVER_WAIT_ACK)  {
		
		if (!(bufferRx.empty())) {
			
			free((bufferRx.begin())->first);
			bufferRx.pop_front();
			check = 1;
		}
	}
	
	if (check) {
		
		Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_MicroModem::clearTxInfo DONE");
	}
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::clearTxInfo exit");
	
	if (check) {
		
		clearTxInfo();
	}
}

/*!
 * 	@brief The clearTxInfo function is clears all the information regarding the last transmission on-going. 
 *	It also stops the modem timer before the timeout.
 */

void Sunset_MicroModem::resetTx() 
{
	int check = 0;
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::resetTx start STATE %d d_status %d", STATE, d_status);
	
	if (timeoutTimer_.busy()) {
		
		timeoutTimer_.stop();
	}
	
	if (!listCycToSend.empty()) {
		
		free(listCycToSend.front());
		listCycToSend.pop_front();
		check = 1;
	}
	
	if (!listPktToSend.empty()) {
		
		free(listPktToSend.front());
		listPktToSend.pop_front();
		check = 1;
	}
	
	if (!pktTxList.empty()) {
		
		Packet* p = getPktTxList();
		
		if (Sunset_Statistics::use_stat() && stat != NULL) {
			
			stat->logStatInfo(SUNSET_STAT_MODEM_TX_ABORTED, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");	
		}
		
		pktTxList.pop_front();
		check = 1;
	}
	
	want_ACK = 0;
	
	if (d_status == MM_DRIVER_WAIT_ACK)  {
		
		if (!(bufferRx.empty())) {
			
			free((bufferRx.begin())->first);
			bufferRx.pop_front();
			check = 1;
		}
	}
	
	if (check) {
		
		Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_MicroModem::resetTx INIT MODEM");	
	}
	
	STATE = MM_S_WAIT_CMD;
	d_status = MM_DRIVER_IDLE;
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::resetTx exit STATE %d", STATE);
	
	if (check) {
		
		resetTx();
	}
	
	return;
}


/*!
 * 	@brief The writeMiniPkt function sends a stream of bytes to the Micro-Modem, which has to be transmitted using a mini-packet.
 *	It also starts waiting a given amount of seeconds before notifying a timeout in the current operation.
 *	@param buf The buffer to be sent.
 *	@param size The  length of the buffer.
 *	@param dest The destination ID.
 * 	@retval 0 in case of error, 1 otherwise.
 */

int Sunset_MicroModem::writeMiniPkt(char* buffer, int size, int dest) 
{
	
	int ww = 0;
	char* buf;
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::writeMiniPkt dest %d", dest);
	
	if ((int)dest == Sunset_Address::getBroadcastAddress()) {
		
		/* it is for broadcast transmissions ... in case of broadcast dst = src */
		dest = MODEM_BROADCAST;	
	}
	
	buf = (char*)malloc(sizeof(char) * UMMAXMSSZ);
	
	if ( buf == NULL ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::writeMiniPkt MALLOC ERROR");
		exit(1);	
	}
	
	memset(buf, '\0', sizeof(char) * (UMMAXMSSZ));
	
	Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_MicroModem::writeMiniPkt dest %d state %d string %s", dest, STATE, (char*)buffer);
	
	mm_messages->creaPktCCMUC(buf, UMMAXMSSZ, getModuleAddress(), dest, (char*)buffer, modem_checkSum);
	
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_MicroModem::writeMiniPkt dest %d string %s", dest, buf);
	
	if (d_status != MM_DRIVER_IDLE) {
		
		Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_MicroModem::writeMiniPkt dest %d state %d string %s ERROR", dest, STATE, (char*)buffer);
		exit(1);
	}
	
	d_status = MM_DRIVER_TX_MINI_PKT;
	STATE = MM_S_WAIT_MUC;
	
	if (writeDataToModem(buf, strlen(buf), TIMEOUT_MM_MINI_PKT) != 1) {
		
		txAborted();
		free(buf);
		
		return 0;
	}
	
	free(buf);
	return 1;
}

/*!
 * 	@brief The writeModem function sends a stream of bytes to the Micro-Modem initializing all the needed variables and data structures.
 *	@param buf The buffer to be sent.
 *	@param size The  length of the buffer.
 *	@param dest The destination ID.
 * 	@retval 0 in case of error, 1 otherwise.
 */

int Sunset_MicroModem::writeModem(char* buffer, int size, int dest) 
{	
	
	int nmea_type = -1;
	
	if (size + PKT_MODEM_OVERHEAD > UMMAXMSSZ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::writeModem TODO we have to handle "
					"packet framing ERROR");
		exit(1);
	}
	
	if (USE_ASCII) {
		
		nmea_type = MM_SEND_ASCII;
	}
	else {
		
		nmea_type = MM_SEND_HEX;
	}
	
	if (!sendData(getModuleAddress(), dest, MODEM_RATE, MM_MODEM_FLAG, MM_MODEM_FRAME, (char*)buffer, USE_ACK, modem_checkSum, nmea_type, size) == -1) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::writeModem write failed ERROR!");
		
		return 0;
	}
	
	return 1;
}

/*!
 * 	@brief The txDone function is called when a packet transmission has been correctly completed.
 *	All the status and temporary data structures are cleared from the information 
 *	regarding the completed transmission.
 */

void Sunset_MicroModem::txDone() 
{
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::txDone d_status %d STATE %d", d_status, STATE);
	
	if (d_status == MM_DRIVER_TX_MINI_PKT || d_status == MM_DRIVER_WAIT_MINI_PKT_XST) {
		
		Packet* p;
		
		if (pktTxList.empty()) {
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::txDone - Something is wrongtx %d ERROR", (int)(pktTxList.size()));
			
			exit(1);
		}
		
		if (timeoutTimer_.busy()) {
			
			timeoutTimer_.stop();
		}
		
		p = getPktTxList();
		pktTxList.pop_front();
		
		Modem2PhyEndTx(p);
		
		Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::txDone MINI_PKT cyc %d send %d tx %d", (int)(listCycToSend.size()), (int)(listPktToSend.size()), (int)(pktTxList.size()));
		
		STATE = MM_S_WAIT_CMD;
		d_status = MM_DRIVER_IDLE;
		
		return;
	}
	
	if (isTxStatus(d_status)) {
		
		Packet* p;
		
		if (listCycToSend.empty() || listPktToSend.empty() || pktTxList.empty()) {
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::txDone - Something is wrong with pktList cyc %d send %d tx %d ERROR", (int)(listCycToSend.size()), (int)(listPktToSend.size()), (int)(pktTxList.size()));
			
			exit(1);
		}
		
		if (timeoutTimer_.busy()) {
			
			timeoutTimer_.stop();
		}
		
		free(*(listCycToSend.begin()));
		free(*(listPktToSend.begin()));
		listCycToSend.pop_front();
		listPktToSend.pop_front();
		
		p = getPktTxList();
		pktTxList.pop_front();
		
		Modem2PhyEndTx(p);
		
		STATE = MM_S_WAIT_CMD;
		d_status = MM_DRIVER_IDLE;
		
		Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::txDone cyc %d send %d tx %d", (int)(listCycToSend.size()), (int)(listPktToSend.size()), (int)(pktTxList.size()));
		
		return;
	}
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::txDone d_status %d STATE %d ERROR", d_status, STATE);
	
	exit(1);
}

/*!
 * 	@brief The clearRxInfo function is clears all the information regarding the last reception on-going.
 */

void Sunset_MicroModem::clearRxInfo() 
{
	int check = 0;
	
	if (!(bufferRx.empty())) {
		
		free((bufferRx.begin())->first);
		bufferRx.pop_front();
		check = 1;
	}
	
	if (receivedPkt != 0) {
		
		Sunset_Utilities::erasePkt(receivedPkt, getModuleAddress());
	}
	
	if (check) {
		
		clearRxInfo();
	}
}

/*!
 * 	@brief The rxAborted function is used when an error occurs during the packet reception. 
 *	It creates an ns-2 packet with the error field of the common header set to 1, and notify such event to the upper layer.
 *	The information regarding the last reception on-going are cleared.
 */

void Sunset_MicroModem::rxAborted(int size) 
{
	if (isRxStatus(d_status)) {
		
		if (timeoutTimer_.busy()) {
			
			timeoutTimer_.stop();
		}
		
		STATE = MM_S_WAIT_CMD;
		d_status = MM_DRIVER_IDLE;
		
		if (Sunset_Statistics::use_stat() && stat != NULL) {
			
			stat->logStatInfo(SUNSET_STAT_MODEM_RX_ABORTED, getModuleAddress(), 0, 0, "");	
		}
		
		clearRxInfo();
		
		Sunset_Generic_Modem::rxAborted(size);	
		
		return;
	}
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::rxAborted d_status %d STATE %d ERROR", d_status, STATE);
	
	exit(1);	
}



/*!
 * 	@brief The sendData function creates and starts the operation to transmit a data message (not a mini-packet) to the Micro-Modem.
 *	@param src The source ID of the message to be transmitted
 *	@param dest The destination ID of the message to be transmitted
 *	@param rate Packet type 0 80 bps FH-FSK
 *							1 250 bps 1/31 spreading (unimplemented)
 *							2 500 bps 1/15 spreading
 *							3 1200 bps 1/7 spreading
 *							4 1300 bps 1/6 rate block code (unimplemented)
 *							5 5300 bps 9/14 rate block code.
 *	@param flag Deprecated. Use either 0 or 1 in this field. To send acknowledgment mini-packets, set the ACK bit in the $CCTXD or $CCTXA messages.
 *	@param skct The number of frames to be used.
 *	@param msg The payload of the message to be sent.
 *	@param ack Set to 1 means that an implicit aknowldegment from the other mdoem (mini-packet) is required.
 *	@param cksum Set to 1 means that checksum has to be calculated and added to the sending packet.
 *	@param packet_type The specific sendData operation: sending MM_SEND_CYC for CCCYC, MM_SEND_ASCII for CCCTXA and MM_SEND_HEX CCCTXD.
 *	@param size The length of the message to be sent.
 * 	@retval 0 in case of error, 1 if the process of starting a trasnmission was correct. 
 *	The return value 1 confirms only the first step of a transmission. Several steps are needed for each transmission and possible errors can occurr in the successive steps.
 */
int Sunset_MicroModem::sendData(int src, int dest, int rate, int flag, int skct,
				char *msg, int ack, int cksum, int packet_type, int size) 
{
	size_t ww = 0;
	char* buf;
	
	if (!checkConnection()) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::sendData but serial port device is not set ERROR");
		
		return 0;
	}
	
	buf = (char*)malloc(sizeof(char) * UMMAXMSSZ);
	
	if ( buf == NULL ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::sendData MALLOC ERROR");
		
		exit(1);
	}
	
	memset(buf, '\0', sizeof(char) * UMMAXMSSZ);			
	
	if ((int)dest == Sunset_Address::getBroadcastAddress()) {
		
		/* it is for broadcast transmissions ... */
		dest = MODEM_BROADCAST;	
	}
	
	switch (packet_type) {
			
		case MM_SEND_CYC:
		{
			
			if( d_status == MM_DRIVER_IDLE  && !listCycToSend.empty() )
			{
				char* aux = *(listCycToSend.begin());
				
				d_status = MM_DRIVER_TX_CYC;
				STATE = MM_S_WAIT_CYC;
				
				if (writeDataToModem(aux, strlen(aux), TIMEOUT_MM_MINI_PKT) != 1) {
					
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::sendData MM_SEND_CYC ERROR writing data");
					
					txAborted();
					
					return 0;
				}
				
				return 1;
			}
			
			Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::sendDATA CYC d_status %d STATE %d ERROR", d_status, STATE);
			
			exit(1);
			
			break;
		}
			
		case MM_SEND_HEX:
		{
			char *msgAux;
			size_t dim;
			char* bufTX;
			
			if( d_status != MM_DRIVER_IDLE) {
				
				Sunset_Debug::debugInfo(2, getModuleAddress(), "sendData SEND HEX but state is not idle %d", d_status);
				exit(1);
			}
			
			d_status = MM_DRIVER_TX_CYC;
			STATE = MM_S_WAIT_CYC;
			
			bufTX = (char*)malloc(sizeof(char) * UMMAXMSSZ);
			
			if( bufTX == NULL ) {				
				
				Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::sendData MALLOC ERROR");
				txAborted();
				
				return 0;
			}
			
			msgAux  = (char *) malloc (sizeof(char)*((size*2)+1));
			
			if( msgAux == NULL ) {
				
				Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::sendData MALLOC ERROR");
				
				txAborted();
				free(bufTX);
				
				return 0;
			}
			memset(msgAux, '\0', sizeof(char) * ((size*2)+1));
			memset(bufTX, '\0', sizeof(char) * UMMAXMSSZ);
			
			dim = hexencode(msgAux, UMMAXMSSZ, msg, size);
			
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::sendData sendData dim: %ld", dim);
			
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::sendData sendData msgAux len %d value %s", strlen(msgAux), msgAux);
			
			mm_messages->creaPktCCTXD(bufTX, UMMAXMSSZ, src, dest, ack, dim, msgAux, cksum);
			
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::sendData sendData bufTX len %d value: %s", strlen(bufTX), bufTX);
			
			free(msgAux);
			
			listPktToSend.push_back(bufTX);
			
			mm_messages->creaPktCCCYC(buf, UMMAXMSSZ, 0, src, dest, rate, USE_ACK, skct, cksum);
			
			listCycToSend.push_back(buf);
			
			if (writeDataToModem(buf, strlen(buf), TIMEOUT_MM_MINI_PKT) != 1) {
				
				Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::sendData MM_SEND_HEX ERROR writing data");
				
				txAborted();
				
				return 0;
			}
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_MicroModem::sendData sendData HEX src %d dst %d rate %d flag %d skct %d", src, dest, rate, flag, skct);
			
			return 1;
			
			break;
		}
			
		case MM_SEND_ASCII:
		{
			char* bufTX;
			
			if( d_status != MM_DRIVER_IDLE) {
				
				Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::sendData SEND ASCII but state is not idle %d", d_status);
				
				exit(1);
			}
			
			d_status = MM_DRIVER_TX_CYC;
			STATE = MM_S_WAIT_CYC;
			
			bufTX = (char*)malloc(sizeof(char) * UMMAXMSSZ);
			
			if( bufTX == NULL ) {		
				
				Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::sendData MALLOC ERROR");
				
				txAborted();
				
				return 0;
			}
			
			memset(bufTX, '\0', sizeof(char) * UMMAXMSSZ);
			
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::sendData ASCII msgAux len %d value %s",  strlen(msg), msg);
			
			mm_messages->creaPktCCTXA(bufTX, UMMAXMSSZ, src, dest, ack, strlen(msg), msg, cksum);
			listPktToSend.push_back(bufTX);
			
			mm_messages->creaPktCCCYC(buf, UMMAXMSSZ, 1, src, dest, rate, USE_ACK, skct, cksum);
			listCycToSend.push_back(buf);
			
			if (writeDataToModem(buf, strlen(buf), TIMEOUT_MM_MINI_PKT) != 1) {
				
				Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::sendData MM_SEND_ASCII ERROR writing data");
				
				txAborted();
				
				return 0;
			}
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_MicroModem::sendData ASCII src %d dst %d rate %d flag %d skct %d", src, dest, rate, flag, skct);
			
			return 1;
			break;
		}
			
		case MM_SEND_DATA:
		{
			
			Sunset_Debug::debugInfo(4, getModuleAddress(), "sendData DRQ: %d", STATE);
			Sunset_Debug::debugInfo(1, getModuleAddress(), "sendData listPktToSend.size %d", (int)(listPktToSend.size()));
			
			if( !listPktToSend.empty() )
			{
				char* aux = (listPktToSend.front());
				
				if (writeDataToModem(aux, strlen(aux), TIMEOUT_MM_DATA) != 1 ) {
					
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::sendData MALLOC ERROR");
					
					txAborted();
					
					return 0;
				}
				
				return 1;
			}
			else {
				
				Sunset_Debug::debugInfo(-1, getModuleAddress(), "TODO we have to check why node has no packet in its list while trying to transmit a pkt ERROR");
			}
			
			break;	
		}
	}
	
	return 0;
}

/*!
 * 	@brief The getTrainigTime function returns the delay for the Micro-Modem training period.
 * 	@retval The training period delay
 */

double Sunset_MicroModem::getTrainigTime()
{
	return 0.5;
}

/*!
 * 	@brief The getAggregateRate function returns the actual bit rate related to a specif packet type
 * 	@param[in] rate The packet type to be coded.
 * 	@retval The bit rate related to the packet type
 */

double Sunset_MicroModem::getAggregateRate(int rate)
{
	if (rate == 0) {
		
		return 80.0;
	}
	
	if (rate == 1) {
		
		return 170.0;
	}
	
	if (rate == 2) {
		
		return 362.0;
	}
	
	if (rate == 5) {
		
		return 795.0;
	}
	
	return dataRate;
}

/*!
 * 	@brief The getCodingOverhead function returns the delay for the Micro-Modem related to coding operations
 * 	@param[in] rate The packet type to be coded.
 * 	@retval The coding delay
 */

double Sunset_MicroModem::getCodingOverhead(int rate)
{
	if (rate == 0) {
		
		return 2;
	}
	
	if (rate == 1) {
		
		return 1;
	}
	
	if (rate == 2) {
		
		return 15;
	}
	
	if (rate == 5) {
		
		return 14.0/9.0;
	}
	
	return 1;
}

/*!
 * 	@brief The getTxTime function computes the time needed by the Micro-Modem to transmit a given number of bytes.
 * 	@param[in] size Number of bytes to be transmitted.
 * 	@retval The transmission delay
 */

double Sunset_MicroModem::getTxTime(int size) 
{
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::getTxTime size %d", size);
	
	return getTrainigTime() + (getPktSize(size)  * 8.0) / getAggregateRate(MODEM_RATE);
}

/*!
 * 	@brief The getSerialTime function computes the time needed to transfer a given number of bytes to the Micro-Modem using the serial line.
 * 	@param[in] size Number of bytes to be transferred.
 * 	@retval The transfer delay
 */

double Sunset_MicroModem::getSerialTime(int size) 
{
	return (size * BAUD_CONV) / (double)baudRate;
}

/*!
 * 	@brief The getPktSize function returns the packet size used by the Micro-Modem when transmitting a given number of 
 *  bytes of information. Pre-defined packet sizes are assumed for the Micro-Modem
 * 	@param[in] dataSize Number of bytes to be transmitted.
 * 	@retval The needed packet size in bytes. 
 */

int Sunset_MicroModem::getPktSize(int dataSize) 
{
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::getPktSize dataSize %d", dataSize);
	
	if (MODEM_RATE == 0) {
		
		return (int)(ceil(dataSize / 32.0) * 32);
	}
	
	if (MODEM_RATE == 1) {
		
		return (int)(ceil(dataSize / 32.0) * 32);
	}
	
	if (MODEM_RATE == 2) {
		
		return (int)(ceil(dataSize / 64.0) * 64);
	}
	
	if (MODEM_RATE == 5) {
		
		return (int)(ceil(dataSize / 256.0) * 256);
	}
	
	return dataSize;
}


/*!
 * 	@brief The createDownCtrlPkt function converts an ns-2 packet into mini-packet for the modem.
 *	@param p The ns-2 packet to be converted. The destination ID of a mini-packet has reserved only 3 bits. 
 *	To increase the number of IDs which can be addressed, we write the destination ID of the ns-2 packet in the payload of the 
 *	mini-packet. In this way the maximum ID value is 15 insetaad of 7.
 *	@param[out] len The length of the mini-packet message for the modem.
 * 	@retval The mini-packet message for the modem after the conversion.
 */

char* Sunset_MicroModem::createDownCtrlPkt(Packet* p, int& len) 
{
	int size = (int)(sizeof(char));
	int dst, src;	
	int aux = 0;
	u_int8_t pType = 0;
	char* buffer;
	char* msgAux;
	int dim = 0;
	
	len = 2;
	
	buffer = (char*)malloc(sizeof(char)*(len+1));
	
	if ( buffer == NULL ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::createDownCtrlPkt MALLOC ERROR");
		exit(1);
	}
	
	memset(buffer, '\0', len+1);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::createDownCtrlPkt total length %d", len);
	
	pktConverter_->getSrc(p, UW_PKT_MAC, src);
	pktConverter_->getDst(p, UW_PKT_MAC, dst);
	
	if (dst > MAX_MINI_PKT_ID) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::createDownCtrlPkt ERROR on dest ID %d", dst);
		exit(1);
	}
	
	pktConverter_->getPktSubType(p, UW_PKT_MAC, aux);
	pType = (u_int8_t)aux;
	
	memcpy(buffer, &(pType), (int)(sizeof(char)));
	memcpy(&(buffer[size]), &(dst), (int)(sizeof(char)));
	
	size += (int)(sizeof(char));
	
	msgAux  = (char *) malloc (sizeof(char)*(UMMAXMSSZ));
	
	if( msgAux == NULL ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::createDownCtrlPkt MALLOC ERROR");
		exit(1);
	}
	
	memset(msgAux, '\0', sizeof(char) * (UMMAXMSSZ));
	
	/* codifica in hex il buffer msgAux */
	
	dim = hexencode(msgAux, UMMAXMSSZ, buffer, len);
	len = dim;
	
	free(buffer);
	
	Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_MicroModem::createDownCtrlPkt  msgAux %s len %d", msgAux, len);
	
	return msgAux;    
}

/*!
 * 	@brief The createUpCtrlPkt function converts a mini-packet received from the modem into an ns-2 packet.
 *	@param src The source ID of the received message
 *	@param buf The message received from the modem.
 * 	@retval The ns-2 packet after the conversion.
 */

Packet* Sunset_MicroModem::createUpCtrlPkt(int src, char* buf) 
{
	char *ret;
	int size = 0;
	u_int8_t subType = 0;
	int dst = 0;
	Packet* p;
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::createUpCtrlPkt src %d", src);
	
	ret = (char *)malloc(sizeof(char)*(5));
	
	if ( ret == NULL ) {
		
		Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::createUpCtrlPkt MALLOC ERROR");
		
		exit(1);
	}
	
	memset(ret,'\0', 5);
	
	hexdecode(ret,UMMAXMSSZ, buf, 4);
	
	size = (int)(sizeof(u_int8_t));
	
	memcpy(&subType, ret, (int)(sizeof(u_int8_t)));
	memcpy(&dst, &(ret[size]), size);
	
	free(ret);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::createUpCtrlPkt chiamo mac src %d dst %d subTpe %d", src, dst, subType);
	
	if (pktConverter_ != 0) {
		
		p = pktConverter_->createMiniPkt(UW_PKT_MAC, src, dst, subType);
	}
	else {
		
		return (Packet*) 0;
	}
	
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::createUpCtrlPkt pkt %d", p);
	
	if (p != 0) {
		
		HDR_CMN(p)->direction() = hdr_cmn::UP;
	}
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::createUpCtrlPkt created");
	
	if (src == getModuleAddress()) {
		
		HDR_CMN(p)->error() = 1;
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::createUpCtrlPkt pkt with ERROR src = myId - MODEM ERROR on miniPkt");
		
		return p;
	}
	
	return p;
}

/*!
 * 	@brief The recvBufferData function is called when a message from the modem has been received and it has to be parsed.
 */

void Sunset_MicroModem::recvBufferData() 
{
	char* buf;
	
	pthread_mutex_lock(&mutex_mm_timer_rx);
	
	if (rxDataBuffer.empty()) {
		
		pthread_mutex_unlock(&mutex_mm_timer_rx);
		
		return;
		
	}
	
	buf = rxDataBuffer.front();
	
	rxDataBuffer.pop_front();
	
	pthread_mutex_unlock(&mutex_mm_timer_rx);
	
	Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_MicroModem::recvBufferData DONE buffer size remaining %d", (int)(rxDataBuffer.size()));
	
	recvPkt(buf);
	
	free(buf);
	
	recvBufferData();
}

/*!
 * 	@brief The readIterate function continuosly listens to the serial line for incoming messages from the modem
 */

int Sunset_MicroModem::readIterate()
{
	char bufRead[UMMAXMSSZ] = {'\0'};
	int len = 0;
	char* pkt;
	
	if (!checkConnection()) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::readIterate but serial port device is not set ERROR");
		
		return -1;
	}
	
	len = mm_conn->read_data(bufRead, UMMAXMSSZ); 
	
	if (len <= 0) {
		
		Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::readIterate NO packet correctly received, len of pkt is: %d", len);
		
		return -1;
	}
	
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_MicroModem::readIterate STATE %d d_state %d string: %s", STATE, d_status, bufRead);
	
	if (Sunset_Statistics::use_stat() && stat != NULL) {
		
		stat->logStatInfo(SUNSET_STAT_MODEM_INFO, getModuleAddress(), 0, 0, "%s", bufRead);	
	}
	
	pkt = (char*) malloc (sizeof(char) * strlen(bufRead)+1);
	
	if ( pkt == NULL ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::readIterate MALLOC ERROR");
		exit(1);	
	}
	
	memset (pkt, '\0', strlen(bufRead) + 1);
	memcpy(pkt, bufRead, strlen(bufRead));	
	
	pthread_mutex_lock(&mutex_mm_timer_rx);
	
	rxDataBuffer.push_back(pkt);
	
	if (!(recvTimer_.busy())) {
		
		recvTimer_.start(0.0001);
	}
	
	pthread_mutex_unlock(&mutex_mm_timer_rx);
	
}

/*!
 * 	@brief The recvPkt function receives a message from the modem, extracts the type of message and executes all the 
 *	operations related to the specific message: Changing in the driver state, reading and writing status variables, etc.
 *	@param buf The message received from the modem.
 * 	@retval The NMEA packet type of the message which has been read.
 */

int Sunset_MicroModem::recvPkt(char * buf)
{
	int pktType = 0;
	
	pktType = mm_messages->NMEAPktType(buf);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::recvPkt pktType %d - buf %s STATE %d status %d", pktType, buf, STATE, d_status);
	
	switch(pktType) {
			
		case NMEA_CAERR:
		{
			caerr_t errore;
			
			if (mm_messages->sscannmea(buf, pktType, &errore) >0)
			{
				STATE = MM_S_WAIT_CMD;
				ERROR_MSG = errore.msg;
				
				if (strncmp(errore.msg, "Bad checksum", strlen(errore.msg)) == 0) {
					
					Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_MicroModem::recvPkt BAD CHECKSUM");
					
					break;
				}
				else if (strncmp(errore.msg, "DATA_TIMEOUT", strlen(errore.msg)) == 0) {
					
					Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_MicroModem::recvPkt DATA Timeout");
					
					if (isTxStatus(d_status)) {
						
						txAborted();	
					}
					else {
						
						clearTxInfo();
					}
					
					break;
				}
				else {
					
					if (isTxStatus(d_status)) {
						
						txAborted();					
					}
					else {
						
						clearTxInfo();
					}
					
					break;
				}
			}
			
			break;
		}
			
		case NMEA_CAMSG:
		{
			camsg_t msg;
			
			if (mm_messages->sscannmea(buf, pktType, &msg) > 0) {
				
				STATE = MM_S_WAIT_CMD;
				MSG_VAL = msg.val;
				MSG_TYPE = msg.type;
				
				if (strcmp(msg.type, "Transmit aborted") == 0) {
					
					Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_MicroModem::recvPkt Transmit aborted");
					
					if (isTxStatus(d_status)) {
						
						txAborted();
					}
					else {
						
						clearTxInfo();
					}
					
					break;
				}
				
				if (strncmp(msg.type, "PACKET_TIMEOUT", strlen(msg.type)) == 0) {
					
					Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_MicroModem::recvPkt Packet Timeout");
					
					
					if (isTxStatus(d_status)) {
						
						txAborted();
					}
					else {
						
						clearTxInfo();
					}
					
					if (isRxStatus(d_status)) {
						
						rxAborted();
					}
					else {
						
						clearRxInfo();
					}
					
					break;
					
				}
				
				if (strcmp(msg.type, "Unknown minipacket type") == 0) {
					
					Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_MicroModem::recvPkt Unknown minipacket type");
					
					break;
				}
			}
			
			if (strncmp(msg.type, "BAD_CRC", strlen(msg.type)) == 0) {
				
				Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_MicroModem::recvPkt BAD_CRC");
				
				if (d_status == MM_DRIVER_SETUP) {
					
					if (timeoutTimer_.busy()) {
						
						timeoutTimer_.stop();
					}
					
					setupRetry++;
					sendSetUpCmd();
					
					break;
				}
				
				if (!use_CST) {
					
					if (isTxStatus(d_status)) {
						
						txAborted();							
						break;
					}
					
					if (isRxStatus(d_status)) {
						
						rxAborted();							
						break;
					}
				}
				break;
				
			}
			
			break;
		}
			
		case NMEA_CADBG:
		{
			cadbg_t dbg;
			
			if (mm_messages->sscannmea(buf, pktType, &dbg) > 0) {
				;
			}
			
			break;
		}
			
		case NMEA_CAREV:
		{
			carev_t rev;
			
			if (mm_messages->sscannmea(buf, pktType, &rev) > 0 )
			{
				if (strcmp(rev.ident, "INIT") == 0) {
					
					Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_MicroModem::recvPkt CAREV INIT %d", d_status);
					
					if (isTxStatus(d_status)) {
						
						txAborted();		
					}
					
					if (isRxStatus(d_status)) {
						
						rxAborted();	
					}
					
					clearTxInfo();
					clearRxInfo();
					
					d_status = MM_DRIVER_IDLE;
					STATE = MM_S_WAIT_CMD;
				}
				else {
					
					memset(modemVersion, '\0', UMMAXVRSZ);
					strncpy(modemVersion, rev.vers, strlen(rev.vers));
				}
			}
			
			break;
		}
			
		case NMEA_CACFG:
		{
			STATE = MM_S_WAIT_CMD;
			
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::recvPkt NMEA_CACFG %d", d_status);
			
			if (d_status == MM_DRIVER_SETUP) {
				
				setUpCmdDone(buf);
			}
			else {
				
				Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt CACFG d_status %d STATE %d ERROR", d_status, STATE);
				
				exit(1);
				
			}
			
			break;
		}
			
		case NMEA_CACLK:
		{
			
			caclk_t clk;
			
			if (mm_messages->sscannmea(buf, pktType, &clk) > 0)
			{
				
				;
			}
			
			break;
		}
			
		case NMEA_CACYC:
		{
			cacyc_t cyc;
			
			if (mm_messages->sscannmea(buf, pktType, &cyc) > 0) {
				
				Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::recvPkt CACYC src %d addr %d", (int)cyc.src, getModuleAddress());
				
				if ((int)cyc.src == (int)getModuleAddress()) { 
					
					Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::recvPkt CACYC for me STATE %d", STATE);
					
					if(d_status == MM_DRIVER_TX_CYC && !(listCycToSend.empty())) {
						
						STATE = MM_S_WAIT_DRQ; 
						
						Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::recvPkt CACYC for me WAIT_DRQ");
					}	
					else {
						
						Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::recvPkt CACYC for me ERROR STATE %d status %d list empty %d", STATE, d_status, listCycToSend.empty());
					}
				}
				else {
					
					Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::recvPkt CACYC for me WAIT_RXD");
					
					if (isTxStatus(d_status)) {
						
						txAborted();
					}
					
					STATE = MM_S_WAIT_RXD;
					
					if (d_status == MM_DRIVER_IDLE || d_status == MM_DRIVER_ERROR) {
						
						Packet* p = Packet::alloc();
						
						Modem2PhyStartRx(p);
						
						d_status = MM_DRIVER_RX_CYC;
						
						if (timeoutTimer_.busy()) {
							
							timeoutTimer_.stop();
						}
						
						timeoutTimer_.start(TIMEOUT_MM_DATA);
						
						if (Sunset_Statistics::use_stat() && stat != NULL) {
							
							stat->logStatInfo(SUNSET_STAT_MODEM_RX_START, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");	
						}
					}
				}
			}
			
			break;
		}
			
		case NMEA_CADRQ:
		{
			cadrq_t drq;
			
			if ( mm_messages->sscannmea(buf, pktType, &drq) > 0) {
				
				if(d_status == MM_DRIVER_WAIT_DRQ) {
					
					STATE = MM_S_WAIT_TXD;
					d_status = MM_DRIVER_TX_DATA;
					
					if (USE_ACK) {
						
						if (drq.dest != MODEM_BROADCAST) {
							
							want_ACK = 1;
						}
						else {
							
							Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt CADRQ want ack but it is broadcast pkt src %d dst %d", drq.src, drq.dest);
						}
					}
					
					Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt CADRQ good state MM_SEND_DATA listSize %d rqFlag %d /*wantAck*/ %d", (int)(listPktToSend.size()), drq.flag, want_ACK);
					
					sendData(0, 0, 0, 0, 0, NULL, 0, 0, MM_SEND_DATA, 0);
					
				} else {
					
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::recvPkt CADRQ wrong state %d status %d ERROR", STATE, d_status);
				}
			}
			
			break;
		}
		case NMEA_CATXP:
		{
			catxp_t txp;
			
			if (mm_messages->sscannmea(buf, pktType, &txp) > 0) {
				
				STATE = MM_S_WAIT_TXF;
				
				if (isTxStatus(d_status)) {
					
					Packet* p;
					
					isTransmitting = 1;
					
					p = getPktTxList();					
					
					if (Sunset_Statistics::use_stat() && stat != NULL) {
						
						stat->logStatInfo(SUNSET_STAT_MODEM_TX_START, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");	
						
					}
					
					Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt CATXP d_status %d STATE %d start trasnmission", d_status, STATE);
				}
				else if (isRxStatus(d_status)) {
					
					Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt CATXP d_status %d STATE %d ERROR", d_status, STATE);
					d_status = MM_DRIVER_ERROR;
					
					exit(1);
				}
				else {
					
					Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt CATXP d_status %d STATE %d modem ERROR", d_status, STATE);
					
					d_status = MM_DRIVER_ERROR;
				}
			}
			
			break;
		}
			
		case NMEA_CATXF:
		{
			catxf_t txf;
			
			if (mm_messages->sscannmea(buf, pktType, &txf) > 0) {
				
				if (use_XST) {
					
					STATE = MM_S_WAIT_XST;
				}
				else {
					
					STATE = MM_S_WAIT_CMD;
				}
				
				isTransmitting = 0;
				
				Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt pkt ack %d use xst %d state %d", want_ACK, use_XST, STATE);
				
				if (d_status == MM_DRIVER_TX_DATA) {
					
					Packet* p = getPktTxList();
					
					if (p != 0) {
						
						if (Sunset_Statistics::use_stat() && stat != NULL) {
							
							stat->logStatInfo(SUNSET_STAT_MODEM_TX_DONE, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");	
							
						}
						
						Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt TX DATA COMPLETED wantAck %d useXst %d STATE %d", want_ACK, use_XST, STATE);
					}
					else {
						
						Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::recvPkt TX COMPLETED DATA PKT but no pkt ERROR");
						
						exit(1);
					}
					
					if (!use_XST) {
						
						Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt TX COMPLETED call txDone");
						
						if (want_ACK) {
							
							STATE = MM_S_WAIT_ACK;
							
							if (timeoutTimer_.busy()) {
								
								timeoutTimer_.stop();
							}
							
							d_status = MM_DRIVER_WAIT_ACK;
						}
						else {
							
							txDone();			
						}
					}
					else {
						
						d_status = MM_DRIVER_WAIT_DATA_XST;
					}
				}
				else if (d_status == MM_DRIVER_TX_MINI_PKT) {
					
					Packet* p = getPktTxList();
					
					if (p != 0) {
						
						if (Sunset_Statistics::use_stat() && stat != NULL) {
							
							stat->logStatInfo(SUNSET_STAT_MODEM_TX_DONE, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");	
							
						}
						
						Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt TX COMPLETED MINI PKT wantAck %d", want_ACK);
					}
					else {
						
						Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::recvPkt TX COMPLETED MINI PKT but no pkt ERROR");
						
						exit(1);
					}
					
					if (!use_XST) {
						
						Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt TX COMPLETED call txDone");
						
						txDone();							
					}
					else {
						
						d_status = MM_DRIVER_WAIT_MINI_PKT_XST;
					}
				}
				else if (d_status == MM_DRIVER_TX_CYC) {
					
					if (!use_XST) {
						
						if (timeoutTimer_.busy()) {
							
							timeoutTimer_.stop();
						}
						
						d_status = MM_DRIVER_WAIT_DRQ;
						
						Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt TXF  TX COMPLETED CYC");
					}
				}
				else if (d_status == MM_DRIVER_ERROR && !use_XST) {
					
					Packet* p = getPktTxList();	
					
					if (p != 0) {
						
						if (Sunset_Statistics::use_stat() && stat != NULL) {
							
							stat->logStatInfo(SUNSET_STAT_MODEM_TX_DONE, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");	
							
						}
						
						Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt TX DATA COMPLETED ERROR wantAck %d useXst %d STATE %d", want_ACK, use_XST, STATE);
						
						txAborted();
					}
					
				}
				else {
					d_status = MM_DRIVER_ERROR;
				}
			}
			
			break;
		}
			
		case NMEA_CATXD:
		{
			catxd_t txd;
			
			if (mm_messages->sscannmea(buf, pktType, &txd) > 0) {
				
				STATE = MM_S_WAIT_TXP;
			}
			
			break;
		}
		case NMEA_CATXA:
		{
			catxa_t txa;
			
			if (mm_messages->sscannmea(buf, pktType, &txa) > 0) {
				
				STATE = MM_S_WAIT_TXP;
			}
			
			break;
		}
		case NMEA_CAXST:
		{
			caxst_t xst;
			
			if (mm_messages->sscannmea(buf, pktType, &xst) > 0) {
				
				if (!use_XST) {
					
					break;
				}
				
				STATE = MM_S_WAIT_CMD;
				
				if (xst.result == 0) { 
					
					if (d_status == MM_DRIVER_TX_CYC) {
						
						Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::recvPkt XST TX COMPLETED %d should be CCCYC", STATE);
						
						if (timeoutTimer_.busy()) {
							
							timeoutTimer_.stop();
						}
						
						d_status = MM_DRIVER_WAIT_DRQ;
					}
					else if (isTxStatus(d_status)) {
						
						Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::recvPkt XST TX COMPLETED %d OK", STATE);
						
						txDone();
					}
					else if (d_status == MM_DRIVER_ERROR) {
						
						Packet* p = getPktTxList();						
						
						if (p != 0) {
							
							if (Sunset_Statistics::use_stat() && stat != NULL) {
								
								stat->logStatInfo(SUNSET_STAT_MODEM_TX_DONE, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");	
								
							}
							
							Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt XST msg ERROR wantAck %d useXst %d STATE %d", want_ACK, use_XST, STATE);
							
							txAborted();
						}
						
					}
					else {
						Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt CAXST d_status %d STATE %d ERROR", d_status, STATE);
						exit(1);
						
					}
				}
				else {
					
					if (isTxStatus(d_status) || d_status == MM_DRIVER_ERROR) {
						
						Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::recvPkt XST TX ABORTED result %d OK", xst.result);
						txAborted();
					}
					else {
						
						Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt CAXST d_status %d STATE %d ERROR", d_status, STATE);
						
						exit(1);
						
					}
				}
				
				Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt TX COMPLETED XST result %d rate %d carrier %d txbytes %d pktType %d STATE %d", xst.result, xst.rate, xst.carrier, xst.txbytes, xst.pktype, STATE);
			}
			
			break;
		}
			
		case NMEA_CARXP:
		{
			carxp_t rxp;
			
			if (mm_messages->sscannmea(buf, pktType, &rxp) > 0) {
				;
			}
			
			break;
		}
		case NMEA_CAACK:
		{
			caack_t ack;
			
			if (mm_messages->sscannmea(buf, pktType, &ack) > 0) {
				
				STATE = MM_S_WAIT_CMD;
				
				if (d_status == MM_DRIVER_WAIT_ACK) {
					
					want_ACK = 0;
					
					
					Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt NMEA_CAACK src %d dst %d", ack.src, ack.dest);
					
					if (ack.dest == getModuleAddress()) {
						
						txDone();
					} 
					else {
						
						Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::recvPkt NMEA_CAACK src %d dst %d state %d ERROR",  ack.src, ack.dest, STATE);
					}
				}				
			}
			
			break;
		}
		case NMEA_CARXD:
		{
			carxd_t rxd;
			char * ret;
			size_t dim;
			
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::recvPkt get hex data");
			
			if (mm_messages->sscannmea(buf, pktType, &rxd) > 0)
			{
				
				if (use_CST) {
					
					STATE = MM_S_WAIT_CST;
				}
				else {
					
					STATE = MM_S_WAIT_CMD;
					
					if (timeoutTimer_.busy()) {
						
						timeoutTimer_.stop();
					}
				}
				
				if (isTxStatus(d_status)) {
					
					txAborted();
					break;
				}
				
				if (!isRxStatus(d_status)) {
					
					clearRxInfo();
					break;
				}
				
				ret = (char *)malloc(sizeof(char)*(rxd.len+1));
				
				if ( ret == NULL ) {
					
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::recvPkt MALLOC ERROR", dim);
					exit(1);
				}
				
				memset(ret,'\0', rxd.len+1);
				
				dim = hexdecode(ret,UMMAXMSSZ, rxd.buf, rxd.len);
				
				if (use_CST) {
					
					pair<char*, int> pair;
					
					pair.first = ret;
					pair.second = dim;
					
					bufferRx.push_back(pair);
					
					Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::recvPkt received  HEX dim %d", dim);
					
					if (d_status == MM_DRIVER_RX_DATA) {
						
						d_status = MM_DRIVER_WAIT_DATA_CST;
					}
					else if (d_status == MM_DRIVER_RX_MINI_PKT) {
						
						d_status = MM_DRIVER_WAIT_MINI_PKT_CST;
					}
					else {
						Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt CARXD d_status %d STATE %d ERROR", d_status, STATE);
						
						exit(1);
					}
				}
				else {
					
					d_status = MM_DRIVER_IDLE;
					
					int aux_dst = rxd.dest;
					
					if (aux_dst == MODEM_BROADCAST) {
						
						aux_dst = Sunset_Address::getBroadcastAddress();
					}
					
					pktReceived(rxd.src, aux_dst, ret, dim);
				}
			}
			
			break;
		}
		case NMEA_CARXA:
		{
			
			carxa_t rxa;	
			char* ret;
			
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::recvPkt get ascii data");
			
			if (mm_messages->sscannmea(buf, pktType, &rxa) > 0) {
				
				if (use_CST) {
					
					STATE = MM_S_WAIT_CST;
				}
				else {
					
					STATE = MM_S_WAIT_CMD;
					
					if (timeoutTimer_.busy()) {
						
						timeoutTimer_.stop();
					}
				}
				
				if (isTxStatus(d_status)) {
					
					txAborted();
					break;
				}
				
				if (!isRxStatus(d_status)) {
					
					clearRxInfo();
					break;
				}
				
				ret = (char *)malloc(sizeof(char)*(rxa.len+1));
				
				if ( ret == NULL ) {
					
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::recvPkt MALLOC ERROR");
					exit(1);
				}
				
				memset(ret,'\0', rxa.len+1);
				memcpy(ret, rxa.buf, rxa.len);
				
				if (use_CST) {
					
					pair<char*, int> pair;
					
					pair.first = ret;
					pair.second = rxa.len;
					
					bufferRx.push_back(pair);
					
					Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::recvPkt received  ASCII dim %d", rxa.len);
					
					if (d_status == MM_DRIVER_RX_DATA) {
						
						d_status = MM_DRIVER_WAIT_DATA_CST;
					}
					
					if (d_status == MM_DRIVER_RX_MINI_PKT) {
						
						d_status = MM_DRIVER_WAIT_MINI_PKT_CST;
					}
				}
				else {
					
					int aux_dst;
					
					d_status = MM_DRIVER_IDLE;
					
					aux_dst = rxa.dest;
					
					if (aux_dst == MODEM_BROADCAST) {
						
						aux_dst = Sunset_Address::getBroadcastAddress();
					}
					
					pktReceived(rxa.src, aux_dst, ret, rxa.len);
				}
			}
			
			break;
		}
			
		case NMEA_CADQF:
		{
			cadqf_t dqf;
			
			if (mm_messages->sscannmea(buf, pktType, &dqf) > 0) {
				
				;
			}
			break;
		}
			
		case NMEA_CAMFD:
		{
			camfd_t mfd;
			
			if (mm_messages->sscannmea(buf, pktType, &mfd) > 0) {
				
				;
			}
			
			break;
		}
			
		case NMEA_CASHF:
		{
			cashf_t shf;
			
			if (mm_messages->sscannmea(buf, pktType, &shf) > 0) {
				
				;
			}
			
			break;
		}
			
		case NMEA_CASNR:
		{
			casnr_t snr;
			
			if (mm_messages->sscannmea(buf, pktType, &snr) > 0) {
				;
			}
			
			break;
		}
			
		case NMEA_CADOP:
		{
			cadop_t dop;
			
			if (mm_messages->sscannmea(buf, pktType, &dop) > 0) {
				;
			}
			
			break;
		}
			
		case NMEA_CACST:
		{
			cacst_t cst;
			
			if (mm_messages->sscannmea(buf, pktType, &cst) > 0) {
				
				STATE = MM_S_WAIT_CMD;
				
				Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_MicroModem::recvPkt received  CACST  state %d status %d",  STATE, d_status);
				
				if (cst.mode == 0) {
					
					if (cst.pktype == 2 || cst.pktype == 4) { //2 = FSK_mini_pkt --- 4 = PSK_mini_pkt
						
						if (d_status == MM_DRIVER_WAIT_MINI_PKT_CST) {
							
							if (receivedPkt != 0) {
								
								Sunset_Generic_Modem::pktReceived(receivedPkt);
								
								receivedPkt = 0;
								
								d_status = MM_DRIVER_IDLE;
								STATE = MM_S_WAIT_CMD;
								
								if (timeoutTimer_.busy()) {
									
									timeoutTimer_.stop();
								}
								
								receivedPkt = 0;
								
								Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt reception CACST MINI_PKT GOOD status STATE %d status %d", STATE, d_status);
								
								break;
							}
							else {
								
								Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::recvPkt reception CACST MINI_PKT GOOD but receivedPkt = 0 ERROR");
								
								exit(1);
							}
						}
						else if (d_status == MM_DRIVER_RX_CYC) {
							
							d_status = MM_DRIVER_RX_DATA;
							
							Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt reception CACST CCCYC GOOD status STATE %d status %d RECEIVING", STATE, d_status);
							
							break;
						}
						else if (d_status == MM_DRIVER_ERROR) {
							
							clearRxInfo();
							
							Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt reception CACST ERROR msg MINI GOOD status STATE %d status %d", STATE, d_status);
							
							break;
						}
						else {
							Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt reception CACST MINI_PKT GOOD ERROR status STATE %d status %d", STATE, d_status);
							
							exit(1);
						}
						
						Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt reception CACST MINI_PKT GOOD status STATE %d status %d", STATE, d_status);
					}
					else {
						
						Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt reception CACST GOOD status %d", d_status);
						
						if (d_status == MM_DRIVER_WAIT_DATA_CST) {
							
							char* ret;
							int dim;
							int aux_dst;
							
							if (bufferRx.empty()) {
								
								Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::recvPkt reception CACST bufferRX EMPTY ERROR");
								
								exit(1);
							}
							
							ret = ((bufferRx.begin())->first);
							dim = ((bufferRx.begin())->second);
							aux_dst = cst.dest;
							
							if (aux_dst == MODEM_BROADCAST) {
								
								aux_dst = Sunset_Address::getBroadcastAddress();
							}
							
							pktReceived(cst.src, aux_dst, ret, dim);
							
							d_status = MM_DRIVER_IDLE;
							
							STATE = MM_S_WAIT_CMD;
							
							if (timeoutTimer_.busy()) {
								
								timeoutTimer_.stop();
							}
							
							Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt reception CACST DATA GOOD status STATE %d status %d", STATE, d_status);
							
							if (!(bufferRx.empty())) {
								
								free((bufferRx.begin())->first);
							}
							
							if (!(bufferRx.empty())) {
								
								bufferRx.pop_front();
							}
							
							break;
						}
						else if (d_status == MM_DRIVER_ERROR) {
							
							clearRxInfo();
							
							Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt reception CACST ERROR msg DATA GOOD status STATE %d status %d", STATE, d_status);
							
							break;
						}
						else {
							Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt reception CACST DATA GOOD ERROR status STATE %d status %d", STATE, d_status);
							
							exit(1);
						}
					}
				}
				else {
					
					Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt reception CACST with problems %d - status %d", cst.result, d_status);						
					if (isTxStatus(d_status)) { 
						
						txAborted();
					}
					if (isRxStatus(d_status)) { 
						
						rxAborted();
					}		
				}
				
				Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt reception CACST rate %d src %d dst %d nfrmaes %d noise %f", cst.rate, cst.src, cst.dest, cst.nframes, cst.stddev);
			}
			
			break;
		}
			
		case NMEA_CAMUC: 
		{
			camuc_t muc;
			
			if (mm_messages->sscannmea(buf, pktType, &muc) > 0) {
			}
			
			break;
		}
			
		case NMEA_CAMUA:
		{
			camua_t mua;
			
			if (mm_messages->sscannmea(buf, pktType, &mua) > 0) {
				
				Packet* p;
				
				if (d_status == MM_DRIVER_SETUP) {
					
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::recvPkt NMEA_CAMUA src %d dst %d state %d sttaus %d BREAK", mua.src, mua.dest, STATE, d_status);
					break;
				}
				
				if (use_CST) {
					
					STATE = MM_S_WAIT_CST;
				}
				else {
					
					STATE = MM_S_WAIT_CMD;
				}
				
				Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt NMEA_CAMUA src %d dst %d use_CST %d", mua.src, mua.dest, use_CST);
				
				if (d_status != MM_DRIVER_IDLE && d_status != MM_DRIVER_ERROR) {
					
					clearRxInfo();
					d_status = MM_DRIVER_IDLE;
				}
				
				p = Packet::alloc();
				
				Modem2PhyStartRx(p);
				
				if (Sunset_Statistics::use_stat() && stat != NULL) {
					
					stat->logStatInfo(SUNSET_STAT_MODEM_RX_START, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");
				}
				
				if (d_status == MM_DRIVER_IDLE || d_status == MM_DRIVER_ERROR) {
					
					receivedPkt = createUpCtrlPkt(mua.src, mua.buf);
					
					if (use_CST) {
						
						STATE = MM_S_WAIT_CST;	//devo pure controllare che sia ascii o hex
						d_status = MM_DRIVER_WAIT_MINI_PKT_CST;
						
						if (timeoutTimer_.busy()) {
							
							timeoutTimer_.stop();
						}
						
						timeoutTimer_.start(TIMEOUT_MM_MINI_PKT);
						
						Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_MicroModem::recvPkt waiting to receive a mini_pkt src %d dst %d use_CST %d receivedPkt == 0 (%d)", mua.src, mua.dest, use_CST, receivedPkt == 0);
					}
					else {
						
						d_status = MM_DRIVER_IDLE;
						STATE = MM_S_WAIT_CMD;
						Sunset_Generic_Modem::pktReceived(receivedPkt);
						
						receivedPkt = 0;
					}
				}
				else {
					
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_MicroModem::recvPkt NMEA_CAMUA src %d dst %d state %d sttaus %d ERROR", mua.src, mua.dest, STATE, d_status);
				}
			}
			
			break;
		}
			
		case NMEA_CAMUR:
		{
			camur_t mur;
			
			if (mm_messages->sscannmea(buf, pktType, &mur) > 0) {
				;
			}
			
			break;
		}
			
		case NMEA_CAMPC:
		{
			campc_t mpc;
			
			if (mm_messages->sscannmea(buf, pktType, &mpc) > 0) {
				;
			}
			
			break;
		}
			
		case NMEA_CAMPA:
		{
			campa_t mpa;
			
			if (mm_messages->sscannmea(buf, pktType, &mpa) > 0) {
				;
			}
			
			break;
		}
			
		case NMEA_CAMPR:
		{
			campr_t mpr;
			
			if (mm_messages->sscannmea(buf, pktType, &mpr) > 0) {
				
				;
			}
			
			break;
		}
			
		case NMEA_CAMEC:
		{
			camec_t mec;
			
			if (mm_messages->sscannmea(buf, pktType, &mec) > 0) {
				
				;
			}
			
			break;
		}
			
		case NMEA_CAMEA:
		{
			camea_t mea;
			
			if (mm_messages->sscannmea(buf, pktType, &mea) > 0) {
				
				;
			}
			
			break;
		}
			
		case NMEA_CAMER:
		{
			camer_t mer;
			if (mm_messages->sscannmea(buf, pktType, &mer) > 0) {
				
				;
			}
			
			break;
		}
			
		case NMEA_CAMSC:
		{
			camsc_t msc;
			
			if (mm_messages->sscannmea(buf, pktType, &msc) > 0) {
				
				;
			}
			
			break;
		}
		case NMEA_CAMSA:
		{
			camsa_t msa;
			
			if (mm_messages->sscannmea(buf, pktType, &msa) > 0) {
				
				;
			}
			
			break;
		}
			
		case NMEA_CAMSR:
		{
			camsr_t msr;                                       
			if (mm_messages->sscannmea(buf, pktType, &msr) > 0) {
				
				;
			}
			
			break;
		}
			
		case NMEA_CARSP:
		{
			carsp_t rsp;
			
			if (mm_messages->sscannmea(buf, pktType, &rsp) > 0) {
				
				;
			}
			
			break;
		}
			
		case NMEA_SNPNT:
		{
			snpnt_t snpnt;
			
			if (mm_messages->sscannmea(buf, pktType, &snpnt) > 0) {
				
				;
			}
			
			break;
		}
			
		case NMEA_SNMFD:
		{
			snmfd_t snmfd;
			
			if (mm_messages->sscannmea(buf, pktType, &snmfd) > 0) {
				;
			}
			
			break;
		}
			
		case NMEA_SNTTA:
		{
			sntta_t sntta;
			
			if (mm_messages->sscannmea(buf, pktType, &sntta) > 0) {
				
				;
			}
			
			break;
		}
			
		case NMEA_SNCFR:
		{
			sncfr_t sncfr;
			
			if (mm_messages->sscannmea(buf, pktType, &sncfr) > 0) {
				
				;
			}
			
			break;
		}
			
		case NMEA_CABBD:
		{
			cabbd_t cabbd;
			
			if (mm_messages->sscannmea(buf, pktType, &cabbd) > 0) {
				;
			}
			
			break;
		}
			
		case NMEA_GPRMC:
		{
			gprmc_t gprmc;
			
			if (mm_messages->sscannmea(buf, pktType, &gprmc) > 0) {
				
				if (USE_GPS_INFO) {
					;
				}
			}
			
			break;
		}
			
		case NMEA_GPGGA:
		{
			gpgga_t gpgga;
			
			if (mm_messages->sscannmea(buf, pktType, &gpgga) > 0) {
				
				;
			}
			
			break;
		}
			
		default:
		{
			break;
		}
	}
	
	return pktType;	
}

/*!
 * 	@brief The rx_thread function starts reading data from the Micro-Modem on the serial line connection.
 */

void Sunset_MicroModem::rx_thread()
{
	
	Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_MicroModem::rx_thread() start");
	
	while(listening == 1) {
		
		Sunset_Utilities::sync_time();
		
		if ( readIterate() == -1 ) {
			
			break;
		}
	}
	
	listening = 0;
	
	disconnect(mm_conn->get_fd());
	
	pthread_mutex_lock(&mutex_mm_timer_connection);
	if ( reconnTimer_.busy() ) {
		
		reconnTimer_.stop();
	}
	
	reconnTimer_.start(0.01);
	pthread_mutex_unlock(&mutex_mm_timer_connection);
	
	Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_MicroModem::rx_thread() exit");
	
	pthread_exit(NULL);
}

