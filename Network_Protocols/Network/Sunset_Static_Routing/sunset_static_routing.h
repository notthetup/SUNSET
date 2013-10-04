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

#ifndef __Sunset_Static_Routing_h__
#define __Sunset_Static_Routing_h__

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
#include <sunset_routing.h>

/*! \brief This class implements the Static Routing protocol. It extends the Sunset_Routing class. 
 * @see class Sunset_Routing
 */

class Sunset_Static_Routing : public Sunset_Routing {
	
public:
	Sunset_Static_Routing();
	virtual int command(int argc, const char*const* argv);
	
protected:
	
	/*! @brief Function called when forwardin a packet p. */
	virtual void forwardPacket (Packet * p);
	
	
};

#endif
