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


#ifndef __Sunset_Trace_h__
#define __Sunset_Trace_h__

#include "random.h"
#include <string>
#include <map>
#include <set>
#include <stdlib.h>
#include <sys/time.h>
#include "config.h"
#include "packet.h"

#define SUNSET_TRACE_MAX_SIZE		2048

/*! \brief This class is used to print trace information. Differently from the Debug class, the message is printed despite the debug level.   */

class Sunset_Trace : public TclObject {
	
public:
	
	Sunset_Trace();
	~Sunset_Trace();
	
	virtual int command( int argc, const char*const* argv );
	
	static Sunset_Trace* instance() 
	{
		
		if (instance_ == NULL) {
			
			Sunset_Trace();
		}
		
		return (instance_);		/*!< General access to debug object */
	}
	
	/*!
	 * 	@brief This is the function of the Trace class use to print out the message to trace, differently from the Debug class, the message is printed despite the debug level. 
	 */
	static void print_info(const char *fmt, ...);
	
	/*!
	 * 	@brief This is the function of the Trace class use for tracing purposes. 
	 */
	static void trace_info(Packet* p, const char *fmt, ...);
	
protected:
	
	/*!
	 * 	@brief This is the function of the Trace class use to print out the input message, it can be extended by subclasses. 
	 */
	virtual void my_print_info(char * str); 
	
	/*!
	 * 	@brief This is the function of the Trace class use to trace the input information, it can be extended by subclasses. 
	 */
	virtual void my_trace_info(Packet* p, char *str);
	
	static Sunset_Trace* instance_;
};

#endif
