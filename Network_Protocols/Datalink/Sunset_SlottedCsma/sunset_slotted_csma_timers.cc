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

#include "sunset_slotted_csma_timers.h"
#include "sunset_slotted_csma.h"

void Sunset_SlottedCsma_Defer_Timer::start(double time)
{
	Scheduler& s = Scheduler::instance();
	
	if (busy_) {
		Sunset_Debug::debugInfo(-1, mac->getModuleAddress(), "Sunset_SlottedCsma_Defer_Timer::start paused ERROR");
		return;
	} 
	
	busy_ = 1;
	paused_ = 0;
	s.sync();
	stime = s.clock();
	rtime = time;
	
	if (rtime < 0.0) {
		
		Sunset_Debug::debugInfo(-1, mac->getModuleAddress(), "Sunset_SlottedCsma_Defer_Timer::start rtime ERROR");
		return;
	} 
	
	Sunset_Utilities::schedule(this, &intr, rtime);
}


void Sunset_SlottedCsma_Defer_Timer::handle(Event *)
{       
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;
	
	((Sunset_SlottedCsma *)mac)->deferHandler();
}

void Sunset_SlottedCsma_Tx_Timer::handle(Event *)
{       
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;
	
	((Sunset_SlottedCsma *)mac)->sendHandler();
}

void Sunset_SlottedCsma_Slot_Timer::handle(Event *)
{
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;
	
	((Sunset_SlottedCsma *)mac)->slotHandler();
}