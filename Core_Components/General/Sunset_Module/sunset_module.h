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

#ifndef __Sunset_Module_h__
#define __Sunset_Module_h__

#include <module.h>
#include <sunset_debug.h>
#include <sunset_information_dispatcher.h>

/*! \brief This class is the main class of all underwater modules. Common properties of underwater modules have to be defined here. The Sunset_Module extends the Sunset_Dispatched_Module to support the functionalities defined by the Information Dispatcher.
 * @see class Sunset_Dispatched_Module
 */

class Sunset_Module : public Sunset_Dispatched_Module {
	
public:
	Sunset_Module() { module_address = -1; run_id = -1; }
	
	/*! \brief  Node ID used during the simulation/emulation */
	
	virtual int getModuleAddress() { return module_address; }
	
	/*! \brief  Experiment ID to be used to log the information in case different nodes are running different experiments */
	
	virtual int getRunId() { return run_id; }
	
	/*! \brief  Function used when the notification about information we are interesting in are updated at the dispatcher module. 
	 * @see Sunset_Information_Dispatcher class for additional information
	 */
	
	virtual int notify_info(list<notified_info> linfo) 
	{ 
		Sunset_Debug::debugInfo(-1, getModuleAddress(), " Sunset_Module::notify_info NOTHING TO DO");
		return 0; 
	} 
	
	virtual void start();	
	virtual void stop();	
	
protected:
	
	int module_address;
	int run_id;
};

#endif
