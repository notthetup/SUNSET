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


#include <sunset_debug.h>
#include <scheduler.h>
#include <iostream>

using namespace std;

Sunset_Debug *Sunset_Debug::instance_ = NULL;
int Sunset_Debug::level = 0;

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_DebugClass : public TclClass {
	
public:
	Sunset_DebugClass() : TclClass("Sunset_Debug") {}
	
	TclObject* create(int, const char*const*) {
		
		return (new Sunset_Debug());
	}
	
} class_Sunset_DebugClass;

Sunset_Debug::Sunset_Debug() : TclObject()
{
	instance_ = this;
}

Sunset_Debug::~Sunset_Debug() 
{
	instance_ = NULL ;
}

/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script.
 *	@param[in] argc argc is a count of the arguments supplied to the command function.
 *	@param[in] argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_Debug::command( int argc, const char*const* argv ) 
{
  	Tcl& tcl = Tcl::instance();
	
	if ( argc == 3 ) {
		
		/* The "setDebug" command sets the debug level, by default its value is 0 */
		
		if (strcmp(argv[1], "setDebug") == 0) {
			
			level = atoi(argv[2]);
			return TCL_OK;
		}	
	}
	
	return TclObject::command(argc, argv);
}


/*!
 * 	@brief This is the function of the Debug class used to print out the debug message. 
 *  It calls the virtual function debug_info to output the appropriate message if running in simulation or emulation mode. When running in emulation mode it also prints out the epoch time.
 * 	@param[in] debugLevel The debug level associated to these information, if debugLevel is higher than class debug level set by the user no information are printed.
 * 	@param[in] addr The node address.
 * 	@param[in] fmt The debug string to print.
 */

void Sunset_Debug::debugInfo(int debugLevel, int addr, const char *fmt, ...)
{
	char command[SUNSET_DEBUG_MAX_SIZE];
	
	if (debugLevel > level) {
		
		return;
	}
	
	memset(command, '\0', SUNSET_DEBUG_MAX_SIZE);
	
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(command, SUNSET_DEBUG_MAX_SIZE, fmt, ap);
	
	(instance())->debug_info(debugLevel, addr, command);
	
	return;
}

/*!
 * 	@brief This is the main function of the Debug class.
 * 	@param[in] debugLevel The debug level associated to these information, if debugLevel is higher than class debug level set by the user no information are printed.
 * 	@param[in] addr The node address.
 * 	@param[in] str The debug string to print.
 */

void Sunset_Debug::debug_info(int debugLevel, int addr, char * str)
{
	char command[SUNSET_DEBUG_MAX_SIZE];
	
	/* When printing the requested information we also update the scheduler clock thus printing correct timing information when running in emulation mode. */
	
	Scheduler& s = Scheduler::instance();
	
	memset(command, '\0', SUNSET_DEBUG_MAX_SIZE);

	if (&s != 0) {
		
		s.sync(); 
	}
	
	if (debugLevel > level) {
		
		return;
	}
	
	snprintf(command, SUNSET_DEBUG_MAX_SIZE, "%d _DEBUG_ (%6.6f) Node:%d - %s", debugLevel, NOW, addr, str);
	
	cout << command << endl;
	
	fflush(stdout);
	
	return;
}


/*!
 * 	@brief This is the function of the Debug class used to print out the debug message despite the debug level and without adding additional information to the string (time and node ID). 
 *  It calls the virtual function show_info which can be extended by subclasses.
 * 	@param[in] str The string to print.
 */

void Sunset_Debug::showInfo(char *str)
{
	
	(instance())->show_info(str);
	
	return;
}



/*!
 * 	@brief This is the function of the Debug class used to print out a message without additional check and information when running in simulation mode.
 * 	@param[in] str The debug string to print.
 */

void Sunset_Debug::show_info(char * str)
{
	
	cout << str << endl;
	
	fflush(stdout);
	
	return;
}


