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
#include <sunset_static_routing.h>
#include <sunset_trace.h>

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_Static_RoutingClass : public TclClass 
{
public:
	
	Sunset_Static_RoutingClass() : TclClass("Module/Sunset_Static_Routing") {}
	
	TclObject* create(int, const char*const*) {
		
		return (new Sunset_Static_Routing());
	}
	
} class_Sunset_Static_RoutingClass;

Sunset_Static_Routing::Sunset_Static_Routing() : Sunset_Routing() 
{
	
}

/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script 
 *	@param[in] argc argc is a count of the arguments supplied to the command function.
 *	@param[in] argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_Static_Routing::command(int argc, const char*const* argv)
{
	
	Tcl& tcl = Tcl::instance();
	
	if (argc == 3) {
		
		if (strcmp(argv[1], "reset_routing_info") == 0) {
			
			if (strcmp(argv[2], "sp") == 0) {
				
				/* Reset the routing table */
				routingTable.clear();
				
				return TCL_OK;
			}
			
			return TCL_ERROR;
		}
	}
	else if (argc == 4) {
		
		/* Add a route in the routing table, (argv[2] is the destination and argv[3] is the relay node */
		
		if (strcasecmp(argv[1], "add_route") == 0) {
			
			int dest = atoi(argv[2]);
			int relay = atoi(argv[3]);
			
			routingTable[dest] = relay;
			
			Sunset_Debug::debugInfo(5, getModuleAddress(), "ROUTING route (dest %d - relay %d) added", dest, relay);
			
			return TCL_OK;
		}
		
		/* Add a route in the routing table, (argv[2] is the destination and argv[3] is the relay node */
		
		if (strcasecmp(argv[1], "nextHop") == 0) {
			
			int dest = atoi(argv[2]);
			int relay = atoi(argv[3]);
			
			routingTable[dest] = relay;
			
			Sunset_Debug::debugInfo(5, getModuleAddress(), "ROUTING nextHop route (dest %d - relay %d) added", dest, relay);
			
			return TCL_OK;
		}
	}
	
	return Sunset_Routing::command(argc, argv);
}

/*!
 * 	@brief The forwardPacket function is invoked when a packet has to be forwarded.
 *	@param p The packet to be forwarded.
 */

void Sunset_Static_Routing::forwardPacket(Packet * p)
{
	
	struct hdr_cmn *cmh = HDR_CMN(p);
	struct hdr_ip *iph = HDR_IP(p);
	int src = 0; 
	int dst = 0; 
	int id = 0;
	
	src = (int)iph->saddr();
	dst = (int)iph->daddr();
	id = cmh->uid();
	
	//if I do not have any relay node for the current packet discard it
	if (iph -> daddr() != Sunset_Address::getBroadcastAddress() && routingTable.find(iph -> daddr()) == routingTable.end()) {
		
		Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_Static_Routing::forwardPacket to node:%d but no route is available, delete pkt", 
					dst);
		
		Sunset_Trace::print_info("rtg - (%f) Node:%d - STATIC no route discard: id %d size %d from %d to %d hops %d\n", NOW, getModuleAddress(), cmh->uid(), cmh->size(), src, dst, cmh->num_forwards());
		
		Sunset_Utilities::erasePkt(p, getModuleAddress());
		
		return;
	}
        
	// forward the packet
	Sunset_Routing::forwardPacket(p);
}

