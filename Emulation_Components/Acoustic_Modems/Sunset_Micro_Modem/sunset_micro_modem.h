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


#ifndef __Sunset_MicroModem_h__
#define __Sunset_MicroModem_h__  

#include <sunset_micro_modem_messages.h>
#include <sunset_generic_modem.h>
#include <sunset_micro_modem_connection.h>

#define MM_MODEM_PORT		1	//Communication port on modem side
#define MM_MODEM_FLAG		0	//DRQ flag for communication set-up (initialization part of each communication host-modem)
#define MM_MODEM_FRAME		1	//number of frame has been transmitted
#define MM_SEND_ASCII		0	//We want to send ASCII data
#define MM_SEND_HEX		1	//We want to send HEX data
#define MM_SEND_CYC		2	//We want to send a formerly queued CCCYC message
#define MM_SEND_DATA		3	//We want to send the data packet to the modem

#define MAX_SETUP_RETRY		3

#define TIMEOUT_MM_SETUP	3
#define TIMEOUT_MM_DATA		6
#define TIMEOUT_MM_MINI_PKT	3	

#define MAX_MINI_PKT_ID		16
#define OFFSET_ID		8	//Node IDs in the network have to be set such that ID + OFFSET_ID < MODEM_BROADCAST && ID_i != ID_j + OFFSET_IF for all i and j in the network 

#define MM_TIMEOUT_CONNECTION 	5

/*!
 @brief This data structure defines the possible states ot the Micro-Modem machine-states
 */

typedef enum  {
	MM_DRIVER_IDLE = 0,
	MM_DRIVER_TX_CYC = 1,
	MM_DRIVER_TX_MINI_PKT = 2,
	MM_DRIVER_TX_DATA = 3,
	MM_DRIVER_RX_MINI_PKT = 4,
	MM_DRIVER_RX_CYC = 5,
	MM_DRIVER_RX_DATA = 6,
	MM_DRIVER_WAIT_DRQ = 7,
	MM_DRIVER_WAIT_MINI_PKT_XST = 8,
	MM_DRIVER_WAIT_DATA_XST = 9,
	MM_DRIVER_WAIT_MINI_PKT_CST = 10,
	MM_DRIVER_WAIT_DATA_CST = 11,
	MM_DRIVER_WAIT_ACK = 12,
	MM_DRIVER_SETUP = 13,
	MM_DRIVER_ERROR = 14
} mm_driver_status;

class Sunset_MicroModem;
class Sunset_Generic_ModemTimer;

class Sunset_MM_TimeoutTimer : public Sunset_Generic_ModemTimer {
public:
	Sunset_MM_TimeoutTimer(Sunset_MicroModem *m) : Sunset_Generic_ModemTimer((Sunset_Generic_Modem*)m) {}
	
	void	handle(Event *e);
};

class Sunset_MM_SetUpTimer : public Sunset_Generic_ModemTimer {
public:
	Sunset_MM_SetUpTimer(Sunset_MicroModem *m) : Sunset_Generic_ModemTimer((Sunset_Generic_Modem*)m) {}
	
	void	handle(Event *e);
};

class Sunset_MM_RecvTimer : public Sunset_Generic_ModemTimer {
public:
	Sunset_MM_RecvTimer(Sunset_MicroModem *m) : Sunset_Generic_ModemTimer((Sunset_Generic_Modem*)m) {}
	
	void	handle(Event *e);
};

class Sunset_MM_ReconnTimer : public Sunset_Generic_ModemTimer {
	
public:
	Sunset_MM_ReconnTimer(Sunset_MicroModem *m) : Sunset_Generic_ModemTimer((Sunset_Generic_Modem*)m) { }
	
protected:
	virtual void handle (Event *e);
	
};

/*! @brief This class implements the FSK Micro-Modem operations. It implements the timing Micro-Modem function defined in Sunset_Modem_Timing_Interface. It extends the generic driver class.
 *  @see class Sunset_Generic_Modem
 */

class Sunset_MicroModem : public Sunset_Generic_Modem {
public:
	
	Sunset_MicroModem();
	
	~Sunset_MicroModem();
	
	friend class Sunset_MM_TimeoutTimer;
	friend class Sunset_MM_SetUpTimer;
	friend class Sunset_MM_RecvTimer;
	friend class Sunset_MM_ReconnTimer;
	
	virtual int command( int argc, const char*const* argv );
	void rx_thread();
	
	virtual double getTxTime(int size);
	virtual double getSerialTime(int size);
	virtual int getPktSize(int dataSize);
	double getAggregateRate(int rate);
	double getTrainigTime();
	double getCodingOverhead(int rate);
	
protected:
	int connect();
	virtual int disconnect(int fd);
	
	int addSetUpCmd(char* cmd, int req);
	int getSetUpInfo(char* buf, char* key, int* val);
	int checkSetUpCmd(char* buf);
	int setUpCmdDone(char* buf);
	int checkConnection();
	int sendSetUpCmd();
	void modemTimeout();
	char* pktWrite(Packet* p, int& len);
	void sendDown(Packet *p);
	
	virtual void txAborted();
	void resetTx();
	
	virtual void txDone();
	virtual void rxAborted(int size = 100);
	
	int sendData(int src, int dest, int rate, int flag, int skct,
		     char *msg, int ack, int cksum, int packet_type, int size);
	int readIterate();
	int recvPkt(char * buf);
	
private:
	int settingModem(int hex, int ascii, int port);
	void sendMiniPkt(Packet *p);
	void sendDataPkt(Packet *p);
	int writeDataToModem(const char *  buf, const size_t len, double timeout);
	int writeMiniPkt(char* buffer, int size, int dest);
	int  writeModem (char* buffer, int size, int dest);
	char* createDownCtrlPkt( Packet* p, int& len);
	Packet* createUpCtrlPkt(int src, char* buf);
	void recvBufferData();
	int isTxStatus(mm_driver_status status);
	int isRxStatus(mm_driver_status status);
	void clearTxInfo();
	void clearRxInfo();
	
	
protected:
	
	int STATE;
	mm_driver_status d_status;
	int modem_checkSum;
	int MODEM_RATE;		/* Modem packet type to use */
	int USE_ASCII;	
	int USE_HEX;
	int USE_GPS_INFO;
	int USE_ACK;		/*Use implicit ACK from the modem */
	int want_ACK;
	int useMiniPkt;		/* Use mini-packet when transmitting really short message which can fit in 2 hex values */
	int AGN_VALUE;		/* AGN value to be set */
	int CTO_VALUE;		/* CTO value to be set */
	int ERROR_VAL;
	char * ERROR_MSG;
	int MSG_VAL;
	char * MSG_TYPE;
	
	list< pair<char*, int> > bufferRx;
	list<char*> listPktToSend;
	list<char*> listCycToSend;
	
	Packet* receivedPkt;
	int isTransmitting;
	
	Sunset_MM_TimeoutTimer timeoutTimer_;
	Sunset_MM_SetUpTimer setupTimer_;
	Sunset_MM_RecvTimer recvTimer_;
	Sunset_MM_ReconnTimer reconnTimer_;
	
	list<char*> rxDataBuffer;
	Sunset_MicroModem_Conn * mm_conn;
	char modemVersion[UMMAXVRSZ];
	list<pair<int, char*> > setupInfo; /* int = msg has to be necessary confirmed - char* msg to send  */
	int setupRetry;
	int use_XST;
	int use_CST;
	MicroModem_Messages* mm_messages;
	int mmControl;
	
};

#endif
