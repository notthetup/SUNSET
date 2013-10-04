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


#ifndef SUNSET_MODEM_TIMING_INTERFACE_H 
#define SUNSET_MODEM_TIMING_INTERFACE_H

#include <tclcl.h>
#include <sunset_timing.h>

/*!< \brief This interface defines the function the driver module has to implement in order to compute timing information. 
 These functions are called by the timing emulation module.
 */
class Sunset_Modem_Timing_Interface
{  
	
public:
	
	Sunset_Modem_Timing_Interface() { }
	
	virtual double getTxTime(int size, sunset_rateType rate = TIMING_DATA_RATE) = 0;
	virtual int getPktSize (int dataSize) = 0;
};


#endif /* SUNSET_DRIVER_INTERFACE_H */
