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



#ifndef __Sunset_Flooding_h__
#define __Sunset_Flooding_h__

#include "sunset_routing.h"
#include <stdlib.h>
#include <module.h>
#include <node-core.h>

class Sunset_Flooding;

/*! @brief The data packet forwarding timer. */

class Sunset_Flooding_Timer : public Handler 
{
public:
	Sunset_Flooding_Timer(Sunset_Flooding* rtg) : node_rtg(rtg) {
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
	
	Sunset_Flooding	*node_rtg;
	int		busy_;
	int		paused_;
	Event		intr;
	double		stime;
	double		rtime;
};

/*! @brief This class implements a Probabilistic Flooding Routing protocol. It extends the Sunset_Routing class.
 * @see class Sunset_Routing
 */

class Sunset_Flooding : public Sunset_Routing 
{
	
public:
	Sunset_Flooding();
	
	friend class Sunset_Flooding_Timer; // timer to delay data transmissions.
	
	virtual int command(int argc, const char*const* argv);
	
	virtual void recv(Packet*);
	
protected:
	
	virtual void start();
	
	virtual void stop();
	
	/*! @brief The forwardPacket function is invoked when a packet has to be forwarded. */
	virtual void forwardPacket (Packet * p);
	
	/*!	@brief The sendPkt() function sends a packet to the lower layer and remove it from the local data structures. */
	void sendPkt();
	
protected:
	double probability_;		/*!< @brief The probability to forward a data packet. */
	int max_forwards_;		/*!< @brief Time To Live of the data packets. */
	double max_random_time_;	/*!< @brief If it is set, a random time is waited before to forward a data packet. */
	
	Sunset_Flooding_Timer* floodingTimer_;
	list<Packet*> pktList; 
	
};

#endif
