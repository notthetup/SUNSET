/* SUNSET - Sapienza University Networking framework for underwater Simulation, Emulation and real-life Testing
 *
 * Copyright (C) 2012 Regents of UWSN Group of SENSES Lab
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
 * The author would like to thank Roberto Petroccia <petroccia@di.uniroma1.it>, which has contributed to the development of this module. 
 */


#ifndef __Sunset_Position_Channel_h__
#define __Sunset_Position_Channel_h__

#include <sunset_module.h>
#include <sunset_debug.h>
#include <sunset_information_dispatcher.h>
#include <sunset_tcp_client.h>
#include "sunset_position.h"

class Sunset_Position_Channel;

class Sunset_Position_ChannelTimer : public Handler {
	
public:
	
	Sunset_Position_ChannelTimer(Sunset_Position_Channel* d) : position(d) 
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
	Sunset_Position_Channel	*position;
	int		busy_;
	int		paused_;
	Event		intr;
	double		stime;	// start time
	double		rtime;	// remaining time
	Packet* p_;
};

/*! @brief This timer checks every "nsPosTick" the current node position from the TCL file. */

class Sunset_Position_ChannelTCLTimer : public Sunset_Position_ChannelTimer {
public:
	
	Sunset_Position_ChannelTCLTimer(Sunset_Position_Channel* d) : Sunset_Position_ChannelTimer(d) {}
	
protected:	
	
	virtual void handle(Event *e);
};

/*! @brief This class is used to provide to the Channel Emulator the current node position. It permits to compute the propagation delays between the nodes and to implements node mobility in emulation mode.
 */

class Sunset_Position_Channel : public Sunset_Position {
public:
	Sunset_Position_Channel();
	~Sunset_Position_Channel();
	
	friend class Sunset_Position_ChannelTCLTimer;
	
	virtual int command(int argc, const char*const* argv );
	
	void start();
	void stop();
	
protected:
	int sockFd;
	char ip_addr[SUNSET_TCP_MAX_IP_LEN];
	int port;
	
	int checkNSPos;
	float nsPosTick;
	
	Sunset_Tcp_Client_Connection* connection;	
	
	int sendSunsetPos(node_position, int);
	
	virtual int connect();
	virtual int disconnect(int gPortFd);
	
	virtual bool getTclPosition();
	virtual bool setTclPosition();
	
private:

	Sunset_Position_ChannelTCLTimer tclTimer;
};

#endif
