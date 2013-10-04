/* SUNSET - Sapienza University Networking framework for underwater Simulation, Emulation and real-life Testing
 *
 * Copyright (C) 2012 Regents of UWSN Group of SENSES Lab
 *
 * Author: Daniele Spaccini - spaccini@di.uniroma1.it
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



#ifndef __Sunset_Position_h__
#define __Sunset_Position_h__

#include <sunset_module.h>
#include <sunset_debug.h>
#include <sunset_information_dispatcher.h>

class Sunset_Position;

/*! @brief This class is used to interact with the other modules to collect and provide node position information. The Information Dispatcher is used for the module interaction.
 */

class Sunset_Position : public TclObject, public Sunset_Module {
	
public:
	Sunset_Position();
	~Sunset_Position();
	
	virtual int getModuleAddress() { return module_address; }
	virtual int command(int argc, const char*const* argv );
	virtual int notify_info(list<notified_info>);
	
	void start();
	void stop();
	
protected:
	
	Sunset_Information_Dispatcher* sid;
	int sid_id;
	
	int module_address;

	void sendPosition(int node);
	
	map<int, node_position> position_info;
	
	virtual bool getTclPosition();
	virtual bool setTclPosition();
	
};

#endif
