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



#include <delay.h>
#include <connector.h>
#include <packet.h>
#include <random.h>

#include <sunset_mac_timers.h>
#include <sunset_mac.h>

void Sunset_Mac_Timer::start(double time)
{
	Scheduler& s = Scheduler::instance();
	
	if (busy()) {
		
		Sunset_Debug::debugInfo(-1, mac->addr(), "Sunset_Mac_Timer::start timer busy ERROR");
		
		if (rtime - stime > time) {
			
			return;
		}
		
		stop();
	} 
	
	busy_ = 1;
	paused_ = 0;
	s.sync();
	stime = s.clock();
	rtime = time;
	assert(rtime >= 0.0);
	
	//events are scheduled using the Sunset_Utilities schedule function, which will call the appropriate scheduler if running in simulation or emulation mode.
	Sunset_Utilities::schedule(this, &intr, rtime);
}

void Sunset_Mac_Timer::stop(void)
{
	Scheduler& s = Scheduler::instance();
	
	if(paused_ == 0)
		s.cancel(&intr);
	
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;
}

