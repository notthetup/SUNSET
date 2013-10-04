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



#ifndef __Sunset_Address_h__
#define __Sunset_Address_h__

#include "random.h"
#include <string>
#include <map>
#include <set>
#include <stdlib.h>
#include "config.h"
#include "packet.h"

/*!< \brief This class is used to define information on node IDs. It is used to minimize the number of bits needed to address nodes in the network when running in emulation mode.  */

class Sunset_Address : public TclObject {
	
public:
	
	Sunset_Address();
	~Sunset_Address();
	
	virtual int command( int argc, const char*const* argv );
	
	static Sunset_Address* instance() {
		
		if (instance_ == NULL) {
			
			Sunset_Address();
		}
		
		return (instance_);		// general access to address object
	}
	
	/*!< \brief This function returns the broadcast ID defined by the user using the TCL script */
	static int getBroadcastAddress() { return broadcastAddress_; }
	
private:
	
	static Sunset_Address* instance_;
	
	static int broadcastAddress_; /*!< \brief  ID used for the broadcast address */
};

#endif
