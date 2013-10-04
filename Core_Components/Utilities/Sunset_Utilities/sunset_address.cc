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

#include <sunset_address.h>

Sunset_Address* Sunset_Address::instance_ = NULL;
int Sunset_Address::broadcastAddress_ = -1;

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_AddressClass : public TclClass 
{
public:
	
	Sunset_AddressClass() : TclClass("Sunset_Address") {}
	
	TclObject* create(int, const char*const*) {
		
		return (new Sunset_Address());
	}
	
} class_Sunset_AddressClass;

Sunset_Address::Sunset_Address()  : TclObject()
{
	instance_ = this;
}

Sunset_Address::~Sunset_Address()
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

int Sunset_Address::command( int argc, const char*const* argv ) 
{
	if ( argc == 3 ) {
		
		/* The "setBroadcastAddress" command sets the broadcast ID used during the simulation/emulation. When running in emulation modem, the ID equal to 0 is commonly used for broadcast address. */
		
		if (strcmp(argv[1], "setBroadcastAddress") == 0) {
			
			broadcastAddress_ = atoi(argv[2]);
			
			return TCL_OK;
		}	
	}
	
	return TclObject::command(argc, argv);
}

