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

#ifndef _Sunset_Evologics_v1_6_h__
#define _Sunset_Evologics_v1_6_h__

#include "sunset_evologics_include.h"
#include "sunset_evologics_connection.h"
#include "sunset_evologics_def.h"

#define EV_ATT_REQUEST_TIME_1_6 	0.2

class Sunset_Evologics_v1_6;
class Sunset_Generic_ModemTimer;

typedef struct time_tx {
	
	double epoch;
	int size;
	int src;
	int dest;
	
} time_tx;

/*! @brief Modem timeout timer. */

class Sunset_Evologics_Timeout_Timer : public Sunset_Generic_ModemTimer {
	
public:
	Sunset_Evologics_Timeout_Timer(Sunset_Evologics_v1_6 *m) : Sunset_Generic_ModemTimer((Sunset_Generic_Modem*)m) { }
	
protected:
	virtual void handle (Event *e);
	
};

/*! @brief Delivery status message timer. */

class Sunset_Evologics_DeliveryStatus_Timer : public Sunset_Generic_ModemTimer {
	
public:
	Sunset_Evologics_DeliveryStatus_Timer(Sunset_Evologics_v1_6 *m) : Sunset_Generic_ModemTimer((Sunset_Generic_Modem*)m) { }
	
protected:
	virtual void handle (Event *e);
	
};

/*! @brief Data burst message response timer. */

class Sunset_Evologics_BurstMsgResp_Timer : public Sunset_Generic_ModemTimer {
	
public:
	Sunset_Evologics_BurstMsgResp_Timer(Sunset_Evologics_v1_6 *m) : Sunset_Generic_ModemTimer((Sunset_Generic_Modem*)m) { }
	
protected:
	virtual void handle (Event *e);
	
};

/*! @brief Modem reply timer for propagation delay estimation. */

class Sunset_Evologics_RangingReply_Timer : public Sunset_Generic_ModemTimer {
	
public:
	Sunset_Evologics_RangingReply_Timer(Sunset_Evologics_v1_6 *m) : Sunset_Generic_ModemTimer((Sunset_Generic_Modem*)m) { }
	
	virtual void start(double time, int dst);
	virtual void stop(void);
	
protected:
	virtual void handle (Event *e);
	int dest;
	
};

/*! @brief Modem reply timer for reply with synchronous instant message. */

class Sunset_Evologics_RTT_Timer : public Sunset_Generic_ModemTimer {
	
public:
	Sunset_Evologics_RTT_Timer(Sunset_Evologics_v1_6 *m) : Sunset_Generic_ModemTimer((Sunset_Generic_Modem*)m) { }
	
protected:
	virtual void handle (Event *e);
	
};

/*! @brief Modem reconnection timer. */

class Sunset_Evologics_Start_Connection : public Sunset_Generic_ModemTimer {
	
public:
	Sunset_Evologics_Start_Connection(Sunset_Evologics_v1_6 *m) : Sunset_Generic_ModemTimer((Sunset_Generic_Modem*)m) { }
	
protected:
	virtual void handle (Event *e);
	
};

/*! @brief Modem timer to proces rx information. */

class Sunset_Evologics_Rx_Timer : public Sunset_Generic_ModemTimer {
	
public:
	Sunset_Evologics_Rx_Timer(Sunset_Evologics_v1_6 *m) : Sunset_Generic_ModemTimer((Sunset_Generic_Modem*)m) { }
	
protected:
	virtual void handle (Event *e);
	
};

/*! @brief This class implements the  Evologics Modem (firmware version 1.6) driver. It extends the generic modem class.
 * @see class Sunset_Generic_Modem
 */

class Sunset_Evologics_v1_6 : public Sunset_Generic_Modem {
public:
	
	friend void *thread_proxy_function(void *);
	
	friend class Sunset_Evologics_Timeout_Timer; 		/*!< @brief modem timeout timer. */
	friend class Sunset_Evologics_DeliveryStatus_Timer;	/*!< @brief delivery status timer. */
	friend class Sunset_Evologics_BurstMsgResp_Timer;	/*!< @brief data burst message response timer. */
	friend class Sunset_Evologics_RangingReply_Timer;	/*!< @brief Modem reply timer for propagation delay estimation. */
	friend class Sunset_Evologics_RTT_Timer;		/*!< @brief Modem reply timer for reply with synchronous instant message. */
	friend class Sunset_Evologics_Start_Connection;		/*!< @brief modem reconnection timer. */
	friend class Sunset_Evologics_Rx_Timer;             	/*!<@brief timer call from listening thread to process received information. */
	
	Sunset_Evologics_v1_6();
	~Sunset_Evologics_v1_6();
	
	virtual int command( int argc, const char*const* argv );
	virtual void resetTx();
	
	Sunset_Evologics_Conn *evo_conn;
	
	virtual void RxIterate(Sunset_Evologics_Conn *);
	
protected:
	
	//Timers
	Sunset_Evologics_Timeout_Timer timeoutTimer_;
	Sunset_Evologics_DeliveryStatus_Timer timeoutDeliv_;
	Sunset_Evologics_BurstMsgResp_Timer timeoutBurstResp_;
 	Sunset_Evologics_RTT_Timer rttTimer_;
	Sunset_Evologics_RangingReply_Timer rangingTimer_;
	Sunset_Evologics_Start_Connection connectionTimer_;
	Sunset_Evologics_Rx_Timer rxTimer_;
	
	virtual void checkSetting();			//control if other modem settings have to be performed
	
	virtual bool set_local_address(int );	//set modem ID
	virtual bool set_promiscous_mode(int );	//set modem promiscous mode
	virtual bool set_modem_power(int ); 	// set modem tx power
	
	int state;		//current state of the driver
	
	int deliveryRetry;	//counter of the number of times checking to complete a transmission
	
	
	virtual bool checkDeliveryStatus();	//check if a packet has been delivered
	
	virtual bool collectMultiPathInfo(int src) ;	//request multi path information to the modem
	
	virtual bool processRxInfo();

	pthread_mutex_t mutex_evo_1_6_timer_rx;
	
	pthread_mutex_t mutex_evo_1_6_timer_connection;


public:
	
	virtual int connect();	//prepare the connection to the modem
	
	virtual void sendDown(Packet *p); // prepare the transmission of a packet to the modem
	
	virtual void pktReceived(char* buffer, int len); // reception of a stream of bytes from the modem
	
	virtual void pktReceived(int src, int dst, char* buffer, int len); // reception of a stream of bytes with src and dest IDs from the modem
	
	virtual char* pktWrite(Packet* p, int& len);	//prepare the conversion of a packet in a stream of bytes
	
	virtual bool start_connection();	//open the connection to the modem	
	virtual bool close_connection();	//close the connection to the modem
	virtual bool start_listener();		//start thread listening to the modem
	
	virtual double getTxTime(int size);	//compute the time needed to transmit size bytes of information
	
	virtual double getTransfertTime(int size); //compute the time needed to transfert size bytes of information to the modem
	
	
	virtual bool send_data_im(char *, int, int, char *);	//send Instant Message
	bool send_data_imRTT();		//send ranging request
	virtual bool send_data_burst(char *, int, int);	//send burst of data
	
	virtual void rxAborted(int size = 100);	//error on reception
	
	virtual void txDone();	//transmission completed
	
	virtual void txAborted();	//error on transmission
	
	virtual void rxDoneIm(char *, int);	//receive instant message
	virtual void rxDoneBurst(char *, int);	//receive burst of data
	virtual void rxFailed(char *, int);	//error on reception
	
	virtual void modemTimeout();		//timeout on modem operation
	
	virtual void modemDeliveryTimeout();	//timeout when checking about modem delivering operation
	virtual void modemBurstMsgRespTimeout();	//timeout on burst data transmission
	void handleConnection();	//used to repeat connection operation in case of error
	
	int useBurst;			/*!< @brief if set to one data are transmitted in burst mode instead of using instan messages. */
	int EV_BROADCAST;		/*!< @brief broadcast address of the Evologics modem. */
	
	
	int range_dest;	// it is the node addressed for ranging computatio when using IM with ACK
	int is_ranging;	//1 if support ranging operation, 0 otherwise
	double rangingTime;	//time to use for peiodic range
	
	void rttTimeout();	//handle the request for RTT time
	bool sendRTT_command();	//transmit a ranging message
 	bool startRanging();	//start ranging operation
	
	virtual bool exit_initial_state();
	
	virtual bool modem_configuration();
	
protected:
	int getState();		//return modem current state
	void setState(int );	//set modem state
	
	virtual void setDelayToNode(int node, double delay_usec);
	
	virtual int startConnection();	//start the connection operation
	virtual int stopConnection();	//stop the connection operation
	
	queue<struct evo_setting> setting_cmd;	//setting information
	virtual bool modem_setting();			//perform the setting of the modem
	
	virtual void start();
	
	/*	The following variables are used for ranging purpose to estimated 
	 *	the propagation delay to the other nodes.
	 */
	map<int, pair<double, double> > roundTripTime;		//It stores for each node the estimated round trip delay in seconds and the timestamp of the computation
	map<int, pair<double, double> > distanceToNode;	//It stores for each node the estimated distance (one way delay divided by 1500m/s)  and the timestamp of the computation
	
	map<int,double> timeSentToNode;	// time a node A sends a packet to node B to calculate the propagation delay between A eand B 
	
	int EV_USE_ACK;	// it uses IM with ACK to compute the propagation delay to a specific (nodeDelay) node
	
	int EV_MULTIPATH;	// 1 if we want to collect multipath information, 0 otherwise
	
	int current_tx_power;	//modem tx power once setting has been completed
	
	virtual int notify_info(list<notified_info> linfo);	 //used by the information dispatcher to notify to the modem module something it is interested in
	
	int setTxPower(int node, int tx_level, double time);	//notify change on tx power to the information dispatcher
	
	int mac_slot;
	
	int evoControl;	//if 1 modem initializationis performed
	
	int evoPower;	//desired power for modem transmission
	
	bool listenMode;	
	
	int already_started;	//1 once modem get disconnected
	
	double tx_time;
	
	list< pair<char*, int > > rx_info;
	
};

void *thread_proxy_function_1_6(void *);

#endif

