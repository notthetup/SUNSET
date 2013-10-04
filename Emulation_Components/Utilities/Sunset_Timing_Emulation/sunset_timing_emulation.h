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


#ifndef __Sunset_Timing_Emulation_h__
#define __Sunset_Timing_Emulation_h__

#include <termios.h>
#include <sunset_timing.h>
#include <sunset_generic_modem.h>

/*! @brief This class is used to compute the correct timing delays when running in emulation mode. It extends the basic Timing class and calls the modem driver function to obtain data related to the specific modem operation. */

class Sunset_Timing_Emulation: public Sunset_Timing {
	
public:
	
	Sunset_Timing_Emulation();
	~Sunset_Timing_Emulation() { }
	
	virtual int command( int argc, const char*const* argv );
	double txtime(int size, sunset_rateType rate = TIMING_DATA_RATE);	
	int getPktSize (int dataSize);
	
private:
	
	Sunset_Generic_Modem* modem_;	
};

#endif
