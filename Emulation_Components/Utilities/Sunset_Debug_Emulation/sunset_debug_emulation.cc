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


#include <sunset_debug_emulation.h>
#include <scheduler.h>
#include <iostream>

using namespace std;

double Sunset_Debug_Emulation::start_time = 0.0;

pthread_mutex_t mutex_debug_event;

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_Debug_EmulationClass : public TclClass {
	
public:
	Sunset_Debug_EmulationClass() : TclClass("Sunset_Debug_Emulation") {}
	
	TclObject* create(int, const char*const*) {
		
		return (new Sunset_Debug_Emulation());
	}
	
} class_Sunset_Debug_EmulationClass;

/*!
 * 	@brief This static class extends the basic Sunset_Debug class. 
 *	It prints out real time information such as the epoch time on the running device
 */

Sunset_Debug_Emulation::Sunset_Debug_Emulation()  : Sunset_Debug()
{
	pthread_mutex_init(&mutex_debug_event, NULL);
	instance_ = this;
}

Sunset_Debug_Emulation::~Sunset_Debug_Emulation() 
{
	pthread_mutex_destroy(&mutex_debug_event);
	instance_ = NULL ;
}

/*!  @brief The getClock function returns the emulation time in seconds reflecting the global timing (epoch). 
 *    @retval The epoch time in seconds. 
 */

double Sunset_Debug_Emulation::getClock()
{
	struct timespec tv;
	double s = 0.0;
	
	clock_gettime(CLOCK_REALTIME, &tv);
	s = tv.tv_sec;
	s += (1e-9 * tv.tv_nsec);
	
	return s;
}

/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script 
 *	@param[in] argc argc is a count of the arguments supplied to the command function.
 *	@param[in] argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_Debug_Emulation::command( int argc, const char*const* argv ) 
{
  	Tcl& tcl = Tcl::instance();
	
	if ( argc == 2 ) {
		
		if (strcmp(argv[1], "start") == 0) { //when starting store the starting_time
			
			start_time = this->getClock();
			return TCL_OK;
		}	
		
		if (strcmp(argv[1], "getClock") == 0) {
			
			double time = this->getClock();
			tcl.resultf("%f", time);
			
			return TCL_OK;
		}	
	}
	
	return Sunset_Debug::command(argc, argv);
}

/*!
 * 	@brief This is the main function of the Debug_Emulation class.
 * 	@param[in] debug_emulationLevel The debug_emulation level associated to these information, if debug_emulationLevel is higher than class debug_emulation level set by the user no information are printed.
 * 	@param[in] addr The node address.
 * 	@param[in] fmt The debug_emulation string to print.
 */

void Sunset_Debug_Emulation::debug_info(int debug_emulationLevel, int addr, char * str)
{
 	Tcl& tcl = Tcl::instance();
	double time = 0.0;
	double aux_time = 0.0;
	char command[SUNSET_DEBUG_MAX_SIZE];
	
	memset(command, '\0', SUNSET_DEBUG_MAX_SIZE);
	
	/* When printing the requested information we also update the scheduler clock thus printing correct timing information when running in emulation mode. */
	
	Scheduler& s = Scheduler::instance();
	
	if (&s != 0) {
		
		pthread_mutex_lock(&mutex_debug_event);
		
		s.sync(); 
		
		pthread_mutex_unlock(&mutex_debug_event);
	}
	
	if (debug_emulationLevel > level) {
		
		return;
	}
	
	time = Sunset_Debug_Emulation::getClock();
	aux_time = max(NOW, time - start_time);
	
	if (start_time == 0) {
		
		aux_time = NOW;
	}
	
	sprintf(command, "%d _DEBUG_ (%6.6f) (%f) Node:%d - %s", debug_emulationLevel, aux_time, time, addr, str);
	
	pthread_mutex_lock(&mutex_debug_event);
	
	cout << command << endl;
	fflush(stdout);
	
	pthread_mutex_unlock(&mutex_debug_event);
	
	return;
}

/*!
 * 	@brief This is the function of the Debug class used to print out a message without additional check and information when running in simulation mode.
 * 	@param[in] str The debug string to print.
 */

void Sunset_Debug_Emulation::show_info(char * str)
{
	
	pthread_mutex_lock(&mutex_debug_event);
	
	cout << str << endl;
	
	fflush(stdout);
	
	pthread_mutex_unlock(&mutex_debug_event);
	
	return;
}
