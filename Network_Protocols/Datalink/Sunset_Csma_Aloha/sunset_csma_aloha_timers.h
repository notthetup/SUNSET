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



#ifndef __Sunset_Csma_Aloha_Backoff_Timer_h__
#define __Sunset_Csma_Aloha_Backoff_Timer_h__

#include <connector.h>
#include <random.h>
#include <timer-handler.h>
#include <scheduler.h>
#include "sunset_mac_timers.h"

/* ======================================================================
 Timers
 ====================================================================== */

class Sunset_Csma_Aloha;
class Sunset_Mac_Timer;

/*! @brief This class implements the backoff timer for the CSMA-Aloha protocol. */

class Sunset_Csma_Aloha_Backoff_Timer : public Sunset_Mac_Timer {
	
public:
	
	Sunset_Csma_Aloha_Backoff_Timer(Sunset_Csma_Aloha *m) : Sunset_Mac_Timer((Sunset_Mac*)m) {}
	
	virtual void handle(Event *e);
	virtual void pause(void);
	virtual void resume(void);
};

#endif

