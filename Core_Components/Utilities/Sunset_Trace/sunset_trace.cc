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


#include <sunset_trace.h>
#include <scheduler.h>
#include <iostream>
#include <sunset_debug.h>

using namespace std;

Sunset_Trace *Sunset_Trace::instance_ = NULL;

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_TraceClass : public TclClass {
	
public:
	Sunset_TraceClass() : TclClass("Sunset_Trace") {}
	
	TclObject* create(int, const char*const*) {
		
		return (new Sunset_Trace());
	}
	
} class_Sunset_TraceClass;

Sunset_Trace::Sunset_Trace()  : TclObject()
{
	instance_ = this;
}

Sunset_Trace::~Sunset_Trace() 
{
	instance_ = NULL ;
}

/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script 
 *	@param[in] argc argc is a count of the arguments supplied to the command function.
 *	@param[in] argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_Trace::command( int argc, const char*const* argv ) 
{
	return TclObject::command(argc, argv);
}

/*!
 * 	@brief This is the function of the Trace class used to print out the message to trace, differently from the Debug class, the message is printed despite the debug level. 
 * 	@param[in] fmt The information to print.
 */

void Sunset_Trace::print_info(const char *fmt, ...)
{
			
	char command[SUNSET_TRACE_MAX_SIZE];
	
	memset(command, '\0', SUNSET_TRACE_MAX_SIZE);
	
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(command, SUNSET_TRACE_MAX_SIZE, fmt, ap);
	
	(instance())->my_print_info(command);
	
	return;
}

/*!
 * 	@brief This is the function of the Trace class used to print out the message to trace, it can be extended by subclasses.
 * 	@param[in] str The information to print.
 */

void Sunset_Trace::my_print_info(char * str)
{
	
	Sunset_Debug::showInfo(str);
	
	return;
}

/*!
 * 	@brief This is the function of the Trace class used to trace simulation/emulation actions according to the packet and additional information in input.
 * 	@param[in] p The packet to trace.
 * 	@param[in] fmt The additional information associated to the packet.
 */

void Sunset_Trace::trace_info(Packet* p, const char *fmt, ...)
{
	
	char command[SUNSET_TRACE_MAX_SIZE];
	memset(command, '\0', SUNSET_TRACE_MAX_SIZE);
	
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(command, SUNSET_TRACE_MAX_SIZE, fmt, ap);
	
	(instance())->my_trace_info(p, command);
	
	return;
}

/*!
 * 	@brief This is the function of the Trace class used to trace simulation/emulation actions according to the packet and additional information in input. It can be extended by subclasses to read additional packet header and information defined by the user.
 * 	@param[in] p The packet to trace.
 * 	@param[in] str The additional information associated to the packet.
 */

void Sunset_Trace::my_trace_info(Packet* p, char * str)
{
	
	//At the moment we only print out the string in input
	
	Sunset_Debug::showInfo(str);
	
	return;
}



