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


#ifndef __Sunset_Debug_Emulation_h__
#define __Sunset_Debug_Emulation_h__

#include "random.h"
#include <string>
#include <map>
#include <set>
#include <stdlib.h>
#include <sys/time.h>
#include "config.h"
#include "packet.h"
#include <sunset_debug.h>

/*! \brief This class is used to print debug information. The user can define the requested debug level using the TCL script. Lower levels correspond to less printed information, higher levels to more information. */

class Sunset_Debug_Emulation : public Sunset_Debug {
	
public:
	
	Sunset_Debug_Emulation();
	~Sunset_Debug_Emulation();
	
	virtual int command( int argc, const char*const* argv );
	
	virtual void debug_info(int debugLevel, int addr, char * str); //print out the given information
	
	virtual void show_info(char * str); //print out the given string
	
	static double getClock();	// return the epoch time
	
private:
	
	static double start_time;
};

#endif
