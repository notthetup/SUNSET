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



#ifndef __Sunset_Mac_Timer_h__
#define __Sunset_Mac_Timer_h__

#include <connector.h>
#include <random.h>
#include <timer-handler.h>
#include <scheduler.h>

#include <sunset_mac.h>

class Sunset_Mac;

/*! @brief This is the generic MAC timer class. Other MACs implementing backoff or other timeouts policies can use or extend this timer class 
 *	to implement their timers and the related timeout conditions.
 */

class Sunset_Mac_Timer : public Handler 
{
	
public:
	Sunset_Mac_Timer(Sunset_Mac* m) : mac(m) 
	{
		busy_ = paused_ = 0; stime = rtime = 0.0;
	}
	
	virtual void handle(Event *e) = 0;
	
	virtual void start(double time);
	virtual void stop(void);
	virtual void pause(void) { assert(0); }
	virtual void resume(void) { assert(0); }
	
	inline int busy(void) { return busy_; }
	inline int paused(void) { return paused_; }
	
	inline double expire(void) 
	{
		Scheduler& s = Scheduler::instance();
		return ((stime + rtime) - s.clock());
	}
	
protected:
	Sunset_Mac	*mac;
	int		busy_;
	int		paused_;
	Event		intr;
	double		stime;	// start time
	double		rtime;	// remaining time
};


#endif

