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

#include "sunset_csma_aloha_timers.h"
#include "sunset_csma_aloha.h"

/* ======================================================================
 Backoff Timer
 ====================================================================== */
void
Sunset_Csma_Aloha_Backoff_Timer::handle(Event *)
{
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;
	((Sunset_Csma_Aloha *)mac)->backoffHandler();
}

void Sunset_Csma_Aloha_Backoff_Timer::pause() {
	Scheduler& s = Scheduler::instance();
	
	s.sync();
	double st = s.clock();
	double rt = stime + rtime - st;
	
	if (busy_ == 0 || paused_) {
		Sunset_Debug::debugInfo(-1, mac->getModuleAddress(), "Sunset_Csma_Aloha_Backoff_Timer::pauser paused ERROR");
		//		return;  //TODO: it's just to avoid exiting the simulation
		exit(28);
	}
	
	paused_ = 1;
	rtime = rt;
	
	
	if (rtime < 0.0) {
		Sunset_Debug::debugInfo(-1, mac->getModuleAddress(), "Sunset_Csma_Aloha_Backoff_Timer::pause rtimer ERROR");
		//		return;  //TODO: it's just to avoid exiting the simulation
		exit(28);
	} 
	
	s.cancel(&intr);
}


void Sunset_Csma_Aloha_Backoff_Timer::resume() {
	Scheduler& s = Scheduler::instance();
	
	if (busy_ == 0 || paused_ == 0) {
		Sunset_Debug::debugInfo(-1, mac->getModuleAddress(), "Sunset_Csma_Aloha_Backoff_Timer::resume paused ERROR");
		//		return;  //TODO: it's just to avoid exiting the simulation
		exit(28);
	}
	
	paused_ = 0;
	s.sync();
	stime = s.clock();
	
	if (rtime < 0.0) {
		Sunset_Debug::debugInfo(-1, mac->getModuleAddress(), "Sunset_Csma_Aloha_Backoff_Timer::resume rtimer ERROR");
		//		return;  //TODO: it's just to avoid exiting the simulation
		exit(28);
	} 
	
	Sunset_Utilities::schedule(this, &intr, rtime);
}

