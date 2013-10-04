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


#ifndef __Sunset_Debug_h__
#define __Sunset_Debug_h__

#include "random.h"
#include <string>
#include <map>
#include <set>
#include <stdlib.h>
#include <sys/time.h>
#include "config.h"
#include "packet.h"

#define SUNSET_DEBUG_MAX_SIZE		2048

/*! 
 *\brief This class is used to print debug information. The user can define the requested debug level using the TCL script. Lower levels correspond to less printed information, higher levels to more information. */

class Sunset_Debug : public TclObject {
	
public:
	
	Sunset_Debug();
	~Sunset_Debug();
	
	virtual int command( int argc, const char*const* argv );
	
	static Sunset_Debug* instance() 
	{
		
		if (instance_ == NULL) {
			Sunset_Debug();
		}
		
		return (instance_);		/*!< General access to debug object */
	}
	
	/*!
	 * 	@brief This is the function of the Debug class use to print out the debug message. 
	 */
	static void debugInfo(int debugLevel, int addr, const char *fmt, ...);
	
	/*!
	 * 	@brief This is the function of the Debug class use to print out a message without checking the debug level or adding additional information for debug purposes. 
	 */
	static void showInfo(char *str);
	
protected:
	
	/*!
	 * 	@brief This is the function of the Debug class use to print out the debug message when running in simulation mode. 
	 */
	virtual void debug_info(int debugLevel, int addr, char * str); 
	
	/*!
	 * 	@brief This is the function of the Debug class use to print out a message without additional check and information when running in simulation mode.
	 */
	virtual void show_info(char *str);
	
	static Sunset_Debug* instance_;
	
	static int level;
	
};

#endif
