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
#include <sunset_routing.h>
#include <sunset_trace.h>

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_RoutingClass : public TclClass 
{
public:
	
	Sunset_RoutingClass() : TclClass("Module/Sunset_Routing") {}
	
	TclObject* create(int, const char*const*) {
		
		return (new Sunset_Routing());
	}
	
} class_Sunset_RoutingClass;

Sunset_Routing::Sunset_Routing() : Module() 
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

int Sunset_Routing::command(int argc, const char*const* argv)
{
	
	Tcl& tcl = Tcl::instance();
	
	if (argc == 2) {
		
		/* The "start" command starts the routing module */
		
		if (strcmp(argv[1], "start") == 0) {
			
			Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_Routing::command start Module");
			
			start();
			
			return TCL_OK;
		}
		
		/* The "stop" command stops the routing module */
		
		if (strcmp(argv[1], "stop") == 0) {
			
			stop();
			
			return TCL_OK;
		}
	}
	else if (argc == 3) {
		
		/*  Set node mobile model. */
		
		if (strcasecmp(argv[1], "tracetarget") == 0) {
			
			return TCL_OK;
		}
		
		/*  Return the relay node in the routing table for the specified node (argv[2]) */
		
		else if (strcasecmp (argv[1], "getNextHop") == 0) {
			
			int dest = atoi(argv[2]);
			
			Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Routing::command getNextHop to %d", dest);
			
			tcl.resultf("%d", getNextHop(dest));
			
			return TCL_OK;
		}
		else if (strcasecmp (argv[1], "setModuleAddress") == 0) {
			
			/* The "setModuleAddress" command sets the ID for the current node. */
			
			module_address = atoi(argv[2]);
			
			Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Routing::command module_address %d", module_address);
			
			return TCL_OK;
		}
	}
	
	return Module::command(argc, argv);
}

/*!
 * 	@brief The getNextHop function returns the next hop node ID on the path to dest node.
 *	@param dest The final packet destination ID.
 *	@retval routingTable[dest] The next hop node ID.
 */

int Sunset_Routing::getNextHop(int dest) 
{
	
	if ( routingTable.find(dest) == routingTable.end() ) {
		
		Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Routing::getNextHop no next_hop available return broadcast address");
		
		return Sunset_Address::getBroadcastAddress();
	}
	
	return routingTable[dest];
}

/*!
 * 	@brief The recv function is invoked when a packet is received from the upper/lower layer.
 *	@param p The received packet.
 */

void Sunset_Routing::recv(Packet *p) 
{	
	struct hdr_ip* iph = HDR_IP(p);
	struct hdr_cmn* cmh = HDR_CMN(p);
	int src = (int)iph->saddr() ;
	int dst = (int)iph->daddr() ;
	
	Sunset_Debug::debugInfo(5, getModuleAddress(),  "Sunset_Routing::recv receiving something id %d size %d from %d to %d", cmh->uid(), cmh->size(), src, dst);
	
	/* Packet coming from the upper layer.*/
	
	if (cmh->direction() == hdr_cmn::DOWN) {
		
		forwardPacket(p);
		
		return;
		
	}
	
	/* If the packet is for me, I sendUp() it to the agent layer.*/
	
	Sunset_Trace::print_info("rtg - (%f) Node:%d - ROUTING recv something id %d size %d from %d to %d hops %d\n", NOW, getModuleAddress(), cmh->uid(), cmh->size(), src, dst, cmh->num_forwards());
	
	if (dst == getModuleAddress() || (dst == (int)Sunset_Address::getBroadcastAddress() && src != getModuleAddress())) {
		
		Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Routing::recv received pkt for %d", dst);
		
		sendUp(p);
		
		return;
	}
	
	/* If the packet is not for me, I forward it to the next relay.*/
	
	forwardPacket(p);
	
	return;
}

/*!
 * 	@brief The forwardPacket function is invoked when a packet has to be forwarded.
 *	@param p The packet to be forwarded.
 */

void Sunset_Routing::forwardPacket(Packet * p)
{
	struct hdr_cmn *cmh = HDR_CMN(p);
	struct hdr_ip *iph = HDR_IP(p);
	
	int src = 0; 
	int dst = 0; 
	int id = 0;
	
	src = (int)iph->saddr();
	dst = (int)iph->daddr();
	id = cmh->uid();
	
	cmh->direction() = hdr_cmn::DOWN;
	
	// if I have already processed this packet return to avoid to forward several times the same packet
	if (pktForwardedInfo.find(src) != pktForwardedInfo.end() && 
	    (pktForwardedInfo[src]).find(id) != (pktForwardedInfo[src]).end()) {
		
		
		Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_Routing::forwardPacket to node:%d DUPLICATE pkt", dst);
		
		Sunset_Trace::print_info("rtg - (%f) Node:%d - ROUTING duplicated pkt not forwarded: id %d size %d from %d to %d hops %d\n", NOW, getModuleAddress(), cmh->uid(), cmh->size(), src, dst, cmh->num_forwards());
		
		Sunset_Utilities::erasePkt(p, getModuleAddress());
		
		return;
	}
	
	// insert this packet in the processed packets structure
	pktForwardedInfo[src].insert(id);
	
	Sunset_Trace::print_info("rtg - (%f) Node:%d - ROUTING forwards pkt: id %d size %d from %d to %d hops %d\n", NOW, getModuleAddress(), cmh->uid(), cmh->size(), src, dst, cmh->num_forwards());
	
	if (dst == (int)Sunset_Address::getBroadcastAddress()) {
		
		cmh->next_hop_ = dst;
		
		Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Routing::forwardPacket to node:%d", cmh->next_hop_);		
		
		sendDown(p);
		
		return;
		
	}
	
	// if no information are in the routing table send the packet in broadcast
	if (routingTable.find(iph -> daddr()) == routingTable.end()) {
		
		Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_Routing::forwardPacket to node:%d but no route is available, send in broadcast", dst);
		
		cmh->next_hop_ = Sunset_Address::getBroadcastAddress();
		
		sendDown(p);
		
		return;
	}
	
	cmh->next_hop_ = routingTable[iph->daddr()];
	
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Routing::forwardPacket to node:%d", cmh->next_hop_);
	
	sendDown(p);
}

/*!
 * 	@brief The start() function can be called from the TCL scripts to execute routing module operations when the simulation/emulation starts.
 */

void Sunset_Routing::start()
{
	return;
}

/*!
 * 	@brief The stop() function can be called from the TCL scripts to execute routing module operations when the simulation/emulation stops.
 */

void Sunset_Routing::stop()
{
	return;
}

int Sunset_Routing::recvSyncClMsg(ClMessage* m)
{
	
	/* If the data packet transmission has been aborted ... */
	Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Routing::recvSyncClMsg mType %d", m->type());
	
	if (m->type() == CLMSG_MAC2RTG_PKT_TRANSMITTED) {
		
		txDone(((ClMsgMac2RtgPktTransmitted*)m)->pkt);
		
		return 0;
	}
	else if(m->type() == CLMSG_MAC2RTG_PKT_DISCARDED) {
		
		txDiscarded(((ClMsgMac2RtgPktTransmitted*)m)->pkt);
		
		return 0;
	}
	else {
		return Module::recvSyncClMsg(m);
	}
}

void Sunset_Routing::txDone(const Packet *p)
{	
	return;
}

void Sunset_Routing::txDiscarded(const Packet *p)
{	
	return;
}
