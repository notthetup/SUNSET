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



#ifndef __Sunset_SlottedCsma_timers_h__
#define __Sunset_SlottedCsma_timers_h__

#include <connector.h>
#include <random.h>
#include <timer-handler.h>
#include <scheduler.h>
#include "sunset_mac_timers.h"

class Sunset_SlottedCsma;
class Sunset_Mac_Timer;

/*! @brief This class implements the slot timer for the Slotted CSMA protocol. */

class Sunset_SlottedCsma_Slot_Timer : public Sunset_Mac_Timer {
public:
	Sunset_SlottedCsma_Slot_Timer(Sunset_SlottedCsma *m) : Sunset_Mac_Timer((Sunset_Mac*)m) {}
	
	virtual void handle(Event *e);
};

/*! @brief This class implements the defer timer for the Slotted CSMA protocol. */

class Sunset_SlottedCsma_Defer_Timer : public Sunset_Mac_Timer {
public:
	Sunset_SlottedCsma_Defer_Timer(Sunset_SlottedCsma *m) : Sunset_Mac_Timer((Sunset_Mac*)m) {}
	
	void start(double);
	void handle(Event *e);
};

/*! @brief This class implements the tx timer for the Slotted CSMA protocol when waiting for an ACK packet. */

class Sunset_SlottedCsma_Tx_Timer : public Sunset_Mac_Timer {
public:
	Sunset_SlottedCsma_Tx_Timer(Sunset_SlottedCsma *m) : Sunset_Mac_Timer((Sunset_Mac*)m) {}
	
	void handle(Event *e);
};


#endif
