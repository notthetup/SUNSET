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


#include <address.h>
#include <sunset_transport.h>

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_TransportClass : public TclClass 
{
public:
	
	Sunset_TransportClass() : TclClass("Module/Sunset_Transport") {}
	
	TclObject* create(int, const char*const*) {
		
		return (new Sunset_Transport());
	}
	
} class_Sunset_TransportClass;

Sunset_Transport::Sunset_Transport() : Module() 
{
	module_address = -1;
	
	// Get variables initialization from the Tcl script
	bind("moduleAddress", &module_address);
}

/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script 
 *	@param[in] argc argc is a count of the arguments supplied to the command function.
 *	@param[in] argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_Transport::command(int argc, const char*const* argv)
{
	
	Tcl& tcl = Tcl::instance();
	
	if (argc == 2) {
		
		/* The "start" command starts the transport module */
		
		if (strcmp(argv[1], "start") == 0) {
			
			Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_Transport::command start Module");
			
			start();
			
			return TCL_OK;
		}
		
		/* The "stop" command stops the transport module */
		
		if (strcmp(argv[1], "stop") == 0) {
			
			stop();
			
			return TCL_OK;
		}
	}
	else if (argc == 3) {
		
		if (strcasecmp (argv[1], "setModuleAddress") == 0) {
			
			/* The "setModuleAddress" command sets the ID for the current node. */
			
			module_address = atoi(argv[2]);
			
			Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Transport::command module_address %d", module_address);
			
			return TCL_OK;
		}
	}
	
	return Module::command(argc, argv);
}

/*!
 * 	@brief The recv function is invoked when a packet is received from the upper/lower layer.
 *	@param p The received packet.
 */

void Sunset_Transport::recv(Packet *p) 
{	
	struct hdr_ip* iph = HDR_IP(p);
	struct hdr_cmn* cmh = HDR_CMN(p);
	
	/* Packet coming from the upper layer.*/
	
	if (cmh->direction() == hdr_cmn::DOWN) {
		
		iph->saddr() = getModuleAddress(); //set my address in the IP header
		
		Sunset_Debug::debugInfo(5, getModuleAddress(),  "Sunset_Transport::recv receiving something id %d size %d from %d to %d", cmh->uid(), cmh->size(), iph->saddr(), iph->daddr());
		
		sendDown(p);
		
		return;
		
	}
	
	/* send packet to the upper layer.*/
	
	sendUp(p);
	
	return;
}

/*!
 * 	@brief The start() function can be called from the TCL scripts to execute transport module operations when the simulation/emulation starts.
 */

void Sunset_Transport::start()
{
	return;
}

/*!
 * 	@brief The start() function can be called from the TCL scripts to execute transport module operations when the simulation/emulation stops.
 */

void Sunset_Transport::stop()
{
	return;
}

