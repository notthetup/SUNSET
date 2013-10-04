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



#ifndef __Sunset_Routing_h__
#define __Sunset_Routing_h__

#include "random.h"
#include <map>
#include <set>
#include <stdlib.h>
#include <module.h>
#include <node-core.h>
#include <ip.h>

#include <sunset_utilities.h>
#include <sunset_module.h>
#include <sunset_routing_pkt.h>
#include <sunset_address.h>
#include <sunset_debug.h>
#include <sunset_mac2rtg-clmsg.h>

/*! \brief The generic Routing layer class - it does not implement any methods to update the routing table. Other routing  solutions have to extend this class and implements these methods. */

class Sunset_Routing : public Module, public Sunset_Module {
	
public:
	Sunset_Routing();
	virtual int command(int argc, const char*const* argv);
	virtual void recv(Packet*);
	virtual int getModuleAddress() { return getRoutingAddress(); }
	virtual int recvSyncClMsg(ClMessage* m);
	
protected:
	virtual void start();
	virtual void stop();
	
	/*! @brief Function called when forwardin a packet p. */
	virtual void forwardPacket (Packet * p);
	
	/*! @brief The getRoutingAddress returns the routing module address. */
	inline int getRoutingAddress() { return module_address; }
	
	/*! @brief The getNextHop returns the ID of the next hop realy for packetd addressed to node dest. */
	int getNextHop(int dest);
	
protected:
	map<int, int> routingTable; // routing table <destination, next hop>
	
	map<int, set<int> > pktForwardedInfo; // keep track of packet already forwarded
	
private:
	virtual void txDone(const Packet *);
	virtual void txDiscarded(const Packet *);
	
};

#endif
