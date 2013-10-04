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

#ifndef __Sunset_Utilities_Emulation_h__
#define __Sunset_Utilities_Emulation_h__

#include <sunset_utilities.h>
#include <sunset_pkt_converter.h>
#include <sunset_real_time_scheduler.h>

/*! @brief This class implements the utilities function used when running in real-time (emulation) mode: Removing and deallocating memory for used data packets and scheduling events. */

class Sunset_Utilities_Emulation: public Sunset_Utilities {
	
public:
	
	Sunset_Utilities_Emulation();
	~Sunset_Utilities_Emulation();
	
	virtual void removeData(Packet* p); //remove the actual memory allcated for the payload
	
	virtual void data_copy(Packet* p, Packet* copy); //copy the payload information
	
	virtual void scheduleEvent(Handler* h, Event* e, double delay); //schedule event using the real time scheduler
	
	virtual double getNOW(); //return the emulation time collected from the real time scheduler
	
	virtual double getEpoch(); //return the epoch time collected from the real time scheduler
	
	virtual int my_pkt_size(Packet* p); //return the amount of bytes needed to transmit the packet p
	
	virtual int my_pkt_size(int size); //return the amount of bytes needed to transmit size bytes of payload
	
	virtual int my_max_pkt_size(); //return the maximum payload size
	
	virtual void start();	//initialize module variables and module references when the module starts
	
	virtual void stop();	//reset module variables when the module stops
	
protected:
	
	Sunset_PktConverter* pc;	//reference to the packet converter
	
	Sunset_RealTimeScheduler* scheduler;	//reference to teh real time scheduler
	
};

#endif
