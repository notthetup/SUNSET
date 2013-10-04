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

/*
 * The author would like to thank Daniele Spaccini <spaccini@di.uniroma1.it>, which has contributed to the development of this module. 
 */

#ifndef __Sunset_Generic_Modem_h__
#define __Sunset_Generic_Modem_h__  

#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/times.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/unistd.h>
#include <getopt.h>
#include <termios.h>
#include <sys/signal.h>
#include <sunset_statistics.h>
#include <sunset_address.h>
#include <sunset_tcp_client.h>

#include <packet.h>
#include <sunset_modem2phy-clmsg.h>
#include <sunset_modem_timing_interface.h>

#include <sunset_utilities.h>
#include <sunset_trace.h>
#include <sunset_module.h>
#include <sunset_pkt_converter.h>
#include <sunset_information_dispatcher.h>

#include <sunset_common_pkt.h>

#define PKT_MODEM_OVERHEAD 0
#define MODEM_BROADCAST		0
#define GM_TIMEOUT_CONNECTION 5

#define GM_PROCESSING_EPSILON  0.001 //in sec

typedef struct rx_data {
	char* data;
	int len;
	double time;
} rx_data;

class Sunset_Generic_Modem;

/*! @brief This is the generic modem timer class. */

class Sunset_Generic_ModemTimer : public Handler {
	
public:
	
	Sunset_Generic_ModemTimer(Sunset_Generic_Modem* d) : modem(d) 
	{
		busy_ = paused_ = 0; stime = rtime = 0.0; p_ = 0;
	}
	
	virtual void handle(Event *e) = 0;
	
	virtual void start(Packet* p, double time);
	virtual void start(double time);
	virtual void stop(void);
	virtual void pause(void) { assert(0); }
	virtual void resume(void) { assert(0); }
	
	inline int busy(void) { return busy_; }
	inline int paused(void) { return paused_; }
	
	inline double expire(void) {
		
		Scheduler& s = Scheduler::instance();
		
		return ((stime + rtime) - s.clock());
	}
	
protected:
	Sunset_Generic_Modem	*modem;
	int		busy_;
	int		paused_;
	Event		intr;
	double		stime;	// start time
	double		rtime;	// remaining time
	Packet* p_;
};

/*! @brief Timer used to notify a packet reception. */

class Sunset_Generic_ModemRxDoneTimer : public Sunset_Generic_ModemTimer {
public:
	
	Sunset_Generic_ModemRxDoneTimer(Sunset_Generic_Modem* d) : Sunset_Generic_ModemTimer(d) {
	}
	
	virtual void handle(Event *e);
};

/*! @brief Modem reconnection timer. */

class Sunset_Generic_ModemConnectionTimer : public Sunset_Generic_ModemTimer {
	
public:
	Sunset_Generic_ModemConnectionTimer(Sunset_Generic_Modem* d) : Sunset_Generic_ModemTimer(d) {}
	
protected:	
	virtual void handle (Event *e);
	
};

/*! @brief This class implements the generic modem operations. 
 * It is responsable for the conversion of the data from ns-2 to the external device and vice versa 
 * using the Sunset_Pkt_Converter class and implements the timing function defined in 
 * Sunset_Generic_ModemTimer. All specific acoustic modem drivers extend this class. 
 */

class Sunset_Generic_Modem : public Module, public Sunset_Module, public Sunset_Modem_Timing_Interface {
public:
	
	Sunset_Generic_Modem();
	
	~Sunset_Generic_Modem();
	
	friend class Sunset_Generic_ModemRxDoneTimer;
	friend class Sunset_Generic_ModemConnectionTimer;
	
	virtual int command( int argc, const char*const* argv );
	
	virtual void recv(Packet *p); // called when a packet is received from the upper layer
	
	virtual void startListening();	//start listening thread on serial line or TCP IP connection
	
	virtual double getTxTime(int size);	//compute the time needed to transmit size bytes of information
	
	virtual double getTxTime(int size, sunset_rateType rate); //compute the time needed to transmit size bytes of information of type rate (DATA or Control)
	
	virtual double getTransfertTime(int size); //compute the time needed to transfert size bytes of information to the device
	
	virtual int getPktSize(int dataSize); 	// compute the actual size when transmitting dataSize bytes of information
	
	int getIntBaudRate() { return baudRate; }
	
	void sendUpPkt(Packet *p); //used to send a packet to the upper layer
	
	virtual void resetTx();	// reset tx packets - no queue is supported 
	
	void Modem2PhyEndTx(const Packet* p);	//notity the upper layer on completeed transmission
	
	void Modem2PhyTxAborted(const Packet* p); //notity the upper layer about error on transmission	 
	
	void Modem2PhyStartRx(const Packet* p);	 //notity the upper layer on the start of a reception
	
protected:
	
	virtual int connect();	// open the connect to the device (channel emulator)
	
	virtual int disconnect(int gPortFd); // close the connect to the device (channel emulator)
	
	virtual void sendDown(Packet *p); // prepare the transmission of a packet to the device (channel emulator)
	
	virtual void pktReceived(char* buffer, int len); // reception of a stream of bytes from the device (channel emulator)
	
	virtual void pktReceived(char* buffer, int len, double rx_time); // reception of a stream of bytes from the device (channel emulator)

	virtual void pktReceived(int src, int dst, char* buffer, int len); // reception of a stream of bytes with src and dest IDs from the device (channel emulator)
	
	virtual void pktReceived(Packet* p); // once the packet has been converted this function notify it ot the upper layer
	
	virtual void rxAborted(int size = 100);	//error on reception
	
	virtual void txDone();	//transmission completed
	
	virtual void txAborted();	//error on transmission
	
	virtual char* pktWrite(Packet* p, int& len);	//prepare the conversion of a packet in a stream of bytes
	
	virtual int writeModem(int fd, char* buffer, int size);	//write the information in input to the device (channel emulator)
	
	virtual int sendUp(Packet *p);	// send the packet to the upper layer
	
	virtual void start(); //initialize Generic modem variable and references to othe modules
	
	virtual void stop();	//clean module initialization
	
	virtual char* pkt2Modem(Packet* p, int& len);	//convert a packet into a stream of bytes
	
	virtual Packet* Modem2Pkt(char* buffer, int len);	//convert a stream of bytes into a packet
	
	virtual Packet* Modem2Pkt(int src, int dst, char* buffer, int len); //convert a stream of bytes int a packet with know src and dest IDs
	
	virtual Packet* getPktTxList(); //return first packet in the tx packet list
	
	virtual Packet* getPktRxList(); //return first packet in the rx packet list
	
	size_t hexencode(char *, const size_t, const char *, const size_t); //code input in exdecimal format
	
	size_t hexdecode(char *, const size_t, const char *, const size_t); // decode input	
	
private:
	unsigned char h(unsigned int);
	unsigned int  b(unsigned char);
	
	char* pktEncoding(char *, int& );
	
	char* pktDecoding(char *, int& );
	
	Sunset_Connection* connection;
	
	char ip_addr[SUNSET_TCP_MAX_IP_LEN];	//IP address
	
	int port;	// ip port
	
	int useTcp;	// 1 if using tcp connection
	
	int useSerial;	//1 if using serial line connection
	
	void handleConnection();	//used to repeat connection operation in case of error
	
	void processRxInfo(); //used to process the received information 
	
protected:
	
	int listening;	// if 1 listening thread is active
	
	int gPortFd;	// File descriptor of the connection to the device (channel emulator)
	
	char* devName;	// serial line  device
	
	int baudRate;	//baudrate for the serila line
	
	double dataRate; //used bit rate
	
	int receiving; 	// 1 if modem is receiving, 0 otherwise
	
	int transmitting; // 1 if modem is transmitting, 0 otherwise
	
	Sunset_PktConverter *pktConverter_; //reference to the packet converter
	
	Packet* pktRx;	//received packet
	
	Sunset_Generic_ModemRxDoneTimer rxDoneTimer;
	Sunset_Generic_ModemConnectionTimer connectionTimer;
	
	list<Packet*> pktRxList;		//tx packet list
	list<Packet*> pktTxList;		//rx packet list
	
	Sunset_Statistics* stat;		//reference to the statistics module
	Sunset_Information_Dispatcher* sid;	//reference to the information dispatcher
	
	int sid_id;
	
	int notify_info(list<notified_info> linfo); //used by the information dispatcher to notify to the Generic modem module something it is interested in
	
	list< rx_data > rx_info;
	
	map<int, double>	start_tx_time;
	map<int, double>	start_rx_time;
	
	bool provideDistanceInfo(int node, double dist, double time);
	bool providePropagationDelayInfo(int node, double delay, double time);
	
	int macSrc, macDst;
	
	int getMacSrc() { return macSrc; }		// get the mac source id
	int getMacDst() { return macDst; }		// get the mac destination id
	
	void setMacSrc(int id) { macSrc = id; }		// set the mac source id
	void setMacDst(int id) { macDst = id; }		// set the destination id
};

#endif
