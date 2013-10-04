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


#include <sunset_statistics.h>

Sunset_Statistics* Sunset_Statistics::single = NULL;
int Sunset_Statistics::useStat = 0;

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_StatisticsClass : public TclClass 
{
public:
	
	Sunset_StatisticsClass() : TclClass("Sunset_Statistics") {}
	
	TclObject* create(int, const char*const*) {
		
		return (new Sunset_Statistics());
	}
	
} class_Sunset_StatisticsClass;

/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script 
 *	@param[in] argc argc is a count of the arguments supplied to the command function.
 *	@param[in] argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_Statistics::command( int argc, const char*const* argv ) 
{
	if ( argc == 3 ) {
		
		if (strcmp(argv[1], "setUseStat") == 0) {
			
			useStat = atoi(argv[2]);
			
			return TCL_OK;
		}	
	}	
	else if ( argc == 2 ) {
		
		/* The "start" command starts the statistics module */
		
		if (strcmp(argv[1], "start") == 0) {
			
			start();
			
			return TCL_OK;
		}	
		
		/* The "stop" command stops the statistics module */
		
		if (strcmp(argv[1], "stop") == 0) {
			
			stop();
			
			return TCL_OK;
		}	
	}	
	
	return TclObject::command(argc, argv);
}

/*!
 * 	@brief The start() function can be called from the TCL script to execute statistic module operations when the simulation/emulation starts.
 */

void Sunset_Statistics::start() 
{	
	Sunset_Debug::debugInfo(5, -1, "Statistics::start useStat %d", useStat);
}

/*!
 * 	@brief The stop() function can be called from the TCL script to execute statistic module operations when the simulation/emulation stops.
 */

void Sunset_Statistics::stop() 
{	
	Sunset_Debug::debugInfo(5, -1, "Statistics::stop useStat %d", useStat);
}

/*!
 * 	@brief The instance function returns a statistics instance.
 */

Sunset_Statistics* Sunset_Statistics::instance() 
{
	if(single == NULL) {
		
		Sunset_Statistics();
	}
	
	return single;
}

Sunset_Statistics::Sunset_Statistics() : TclObject() 
{
	single = this;
}

/*!
 * 	@brief The logStatInfo function stores the information related to a specified packet for a specfied action by a node at a given time.
 *	@param sType The action to trace: Transmission, reception, etc.
 *	@param node The node tracing the information.
 *	@param p The packet to trace.
 *	@param time The time related to the action to trace.
 *	@param fmt Additional information to be traced.
 */

void Sunset_Statistics::logStatInfo(sunset_statisticType sType, u_int16_t node, Packet*p, double time, const char *fmt, ...) 
{
	return;
}


