/* SUNSET - Sapienza University Networking framework for underwater Simulation, Emulation and real-life Testing
 *
 * Copyright (C) 2012 Regents of UWSN Group of SENSES Lab <http://reti.dsi.uniroma1.it/SENSES_lab/>
 *
 * Author: Daniele Spaccini - spaccini@di.uniroma1.it
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
 * A special thanks go to Roberto Petroccia <petroccia@di.uniroma1.it> which has contributed to the development of this module. 
 */

#ifndef __Sunset_Channel_Emulator_h__
#define __Sunset_Channel_Emulator_h__

#include <fcntl.h>
#include <fstream>
#include <stdlib.h>
#include <map>
#include <list>
#include <set>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <module.h>
#include <node-core.h>
#include <sunset_module.h>
#include <sunset_tcp.h>

#include <sunset_information_dispatcher.h>
#include <sunset_channel_emulator_connection.h>

#define SUNSET_CH_MAX_SIZE 		SUNSET_TCP_MAX_BUF_SIZE
#define REQUEST_TIME 			5.0
#define ADDRESS_SIZE 			256
#define SERVER_ALREADY_RUNNING		-2

#define CHANNEL_PROP_DELAY 		1500

class Sunset_Channel_Emulator;

struct ch_info {
	
	Sunset_Channel_Emulator *p;
	int sock;
	
} ch_info;

class Sunset_Channel_Emulator_Timer : public Handler {
public:
	Sunset_Channel_Emulator_Timer(Sunset_Channel_Emulator* t) : t_(t) {
		busy_ = paused_ = 0; stime = rtime = 0.0;
	}
	
	virtual void handle(Event *e);
	
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
	Sunset_Channel_Emulator *t_;
	
	int		busy_;
	int		paused_;
	Event		intr;
	double		stime;	// start time
	double		rtime;	// remaining time
	int 		cmdId;
	
public:
	int src, dst;
	int len;
	int id;
	
	char *payload;
};

/*! @brief This class implements the  SUNSET Channel Emulator. 
 * It can be used to test the protocol solutions and parameter settings with the system running in emulation mode 
 * without the need of a real acoustic device connected for communication. It allows to have multiple nodes running
 * in emulation mode on the same machine or in the same netwrok. First node creates and starts the channel emulator,
 * the other nodes insted connect to the instance alread running. Propagation delays among the nodes are emulated
 * according to the defined node positions. Node positions can also be updated over time to emualte the presence of
 * mobile assets in the network.
 */

class Sunset_Channel_Emulator:  public Sunset_Module, public TclObject {
	
	friend class Sunset_Channel_Emulator_Timer;
	
public:
	Sunset_Channel_Emulator();
	~Sunset_Channel_Emulator();
	virtual int command( int argc, const char*const* argv );
	
	void listening_thread(int ) ;
	void listening_thread_pos(int ) ;
	void startModule();
	void startModulePosition();
	
	void sendUp(int, int, char *, int, int );
	
protected:
	
	int createListeningSocket(int socket_port) ;
	void waitForConnections() ;
	void waitForConnectionsPos();
	void closeConnection();
	void freeAllData();
	
	int getModuleAddress() { return ch_address; }
	void start();
	void stop();
		
	int socketPort;		/*!< \brief  Channel emulator server port. */
	int socketPortPos;	/*!< \brief  Positions server port. */
	
	pthread_t startModuleThreadId;
	pthread_t startModulePosThreadId;
	
private:
	int ch_address;
	double defPropDelay;			/*!< \brief  Default propagation delay */
	map<int, int> idSock; 			/*!< \brief <node_id, socket_id>  */
	map<int, node_position> nodePositions; /*!< \brief <node_id, node_position>  */
	
	map<int, map<int, Sunset_Channel_Emulator_Timer *> > nodeTimer; /*!< \brief Timer related to each packet sent <src, <id_timer, timer> >  */
	
	double bitRate;
	
	int socketIdPos;
	int socketId;
	
	double getNodesDelay(int, int );
	double getNodesDistance(node_position, node_position);
	double getCartDistance(node_position, node_position );
	void startSendingTimer(int, int, char *, int );
	int removeIdSock(int );
	void removeClient(int );
	void removeClientPos(int );
	
	int posEmu;		/*!< \brief  1, if the node positions are emulated */
	
	Sunset_Channel_Emulator_Conn *server;
	Sunset_Channel_Emulator_Conn *serverPos;
};

#endif