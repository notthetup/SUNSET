/* SUNSET - Sapienza University Networking framework for underwater Simulation, Emulation and real-life Testing
 *
 * Copyright (C) 2012 Regents of UWSN Group of SENSES Lab
 *
 * Author: Daniele Spaccini - spaccini@di.uniroma1.it
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

#include <sunset_position.h>
#include <sunset_utilities.h>

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_PositionClass : public TclClass 
{
public:
	Sunset_PositionClass() : TclClass("Module/Sunset_Position") {}
	
	TclObject* create(int, const char*const*) {
		
		return (new Sunset_Position());
	}
	
} class_Sunset_Position;


Sunset_Position::Sunset_Position()
{
	module_address = -1;
		
	Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Position::Sunset_Position CREATED");
}

Sunset_Position::~Sunset_Position() 
{
}

/*!
 * 	@brief The start() function can be called from the TCL script to execute position module operations when the simulation/emulation starts.
 */

void Sunset_Position::start() 
{
	Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Position::start");
	
	Sunset_Module::start();
	
	sid = Sunset_Information_Dispatcher::instance();
	
	if (sid != NULL) {
		
		sid_id = sid->register_module(getModuleAddress(), "POSITION", this);
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Position::start sid_id %d", sid_id);

		sid->define(getModuleAddress(), sid_id, "NODE_POSITION");
		sid->define(getModuleAddress(), sid_id, "NODE_POSITION_REQ");
		
		sid->subscribe(getModuleAddress(), sid_id, "NODE_POSITION");
		sid->subscribe(getModuleAddress(), sid_id, "NODE_POSITION_REQ");
		
		sid->provide(getModuleAddress(), sid_id, "NODE_POSITION");	
	}
	
	getTclPosition();
	
}

/*!
 * 	@brief The stop() function can be called from the TCL script to execute position module operations when the simulation/emulation stops.
 */

void Sunset_Position::stop() 
{
	sid = NULL;
	
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Position::stop");
}

/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script 
 *	@param[in] argc argc is a count of the arguments supplied to the command function.
 *	@param[in] argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_Position::command(int argc, const char*const* argv) 
{
	if ( argc == 2 ) {
		
		/* The "start" command starts the Position module */
		
		if (strcmp(argv[1], "start") == 0) {
			
			start();
			
			return TCL_OK;
		}
		
		/* The "stop" command stops the Position module */
		
		if (strcmp(argv[1], "stop") == 0) {
			
			stop();
			
			return TCL_OK;
		}		
	}	
	else if ( argc == 3 ) {
		
		/* The "setModuleAddress" command sets the address of the Position module. */
		
		if (strcmp(argv[1], "setModuleAddress") == 0) {
			
			module_address = atoi(argv[2]);
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Position::command setModuleAddress %d", getModuleAddress());
			
			return (TCL_OK);
		}
		
	}
	
	return TclObject::command( argc, argv );
}


/*!
 * 	@brief The notify_info() function permits to the position module to know the current node SUNSET position.
 * 	@retval 0 in case of error, 1 otherwise.
 */

int Sunset_Position::notify_info(list<notified_info> linfo) 
{ 
	list<notified_info>::iterator it = linfo.begin();
	notified_info ni;
	string s;
	node_position my_pos;
	
	for (; it != linfo.end(); it++) {
		
		ni = *it;
		
		s = "NODE_POSITION_REQ";
		
		if (strncmp((ni.info_name).c_str(), s.c_str(), strlen(s.c_str())) == 0 ) {
			
			if (ni.node_id == getModuleAddress()) {
				
				getTclPosition();
			}

			sendPosition(ni.node_id);				
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Position::notify_info Received request for %d", ni.node_id);	
			
			continue;
			
		}

		s = "NODE_POSITION";
		
		if (strncmp((ni.info_name).c_str(), s.c_str(), strlen(s.c_str())) == 0 ) {
			
			sid->get_value(&my_pos, ni);

			position_info[ni.node_id] = my_pos;
			
			if (ni.node_id == getModuleAddress()) {
				
				if (setTclPosition() == false) {
					
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Position::notify_info position ERROR");	
					
					return 0;
				}
				
			}
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Position::notify_info Received from %d position (%f, %f, %f)", ni.node_id, my_pos.latitude, my_pos.longitude, my_pos.depth);	
		}
	}
	
	Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_Position::notify_info NOTHING TO DO");
	
	return 1; 
}

/*!
 * 	@brief Set the node position to the position module defined in the TCL file.
 */

bool Sunset_Position::setTclPosition() 
{
	Tcl& tcl = Tcl::instance();
	char command[100];
	int ret = 0;
	
	if (position_info.find(getModuleAddress()) == position_info.end()) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Position::setTclPosition no info ERROR");
		
		return false;
	}
	
	node_position my_pos = position_info[getModuleAddress()];
	
	memset(command, '\0', 100);
	sprintf(command, "setSunsetLatitude %d %f", getModuleAddress(), my_pos.latitude);
	ret = Tcl_GlobalEval(tcl.interp(), command);
	
	if (ret == TCL_ERROR) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Position::setTclPosition An error calling %s occured", command);
		
		return false;
	}
		
	memset(command, 0x0, 100);
	sprintf(command, "setSunsetLongitude %d %f", getModuleAddress(), my_pos.longitude);
	ret = Tcl_GlobalEval(tcl.interp(), command);
	
	if (ret == TCL_ERROR) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Position::setTclPosition An error calling %s occured", command);
		
		return false;
	}
	
	
	memset(command, 0x0, 100);
	sprintf(command, "setSunsetDepth %d %f", getModuleAddress(), my_pos.depth);
	ret = Tcl_GlobalEval(tcl.interp(), command);
	
	if (ret == TCL_ERROR) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Position::setTclPosition An error calling %s occured", command);
		
		return false;
	}
		
	Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Position::setTclPosition Node Position: %f %f %f", my_pos.latitude, my_pos.longitude, my_pos.depth);
		
	return true;
}

/*!
 * 	@brief Collect the node position from the position module defined in the tcl file.
 */

bool Sunset_Position::getTclPosition() 
{
	Tcl& tcl = Tcl::instance();
	char command[50];
	int ret = 0;
	node_position my_pos;
	
	memset(command, '\0', 50);
	sprintf(command, "getSunsetLatitude %d", getModuleAddress());
	ret = Tcl_GlobalEval(tcl.interp(), command);
	
	if (ret == TCL_ERROR) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Position::getTclPosition An error calling %s occured", command);
		
		return false;
	}
	
	my_pos.latitude = atof(tcl.result());
	
	memset(command, 0x0, 50);
	sprintf(command, "getSunsetLongitude %d", getModuleAddress());
	ret = Tcl_GlobalEval(tcl.interp(), command);
	
	if (ret == TCL_ERROR) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Position::getTclPosition An error calling %s occured", command);
		
		return false;
	}
	
	my_pos.longitude = atof(tcl.result());
	
	memset(command, 0x0, 50);
	sprintf(command, "getSunsetDepth %d", getModuleAddress());
	ret = Tcl_GlobalEval(tcl.interp(), command);
	
	if (ret == TCL_ERROR) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Position::getTclPosition An error calling %s occured", command);
		
		return false;
	}
	
	my_pos.depth = atof(tcl.result());
	
	Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Position::getTclPosition Node Position: %f %f %f", my_pos.latitude, my_pos.longitude, my_pos.depth);
	
	position_info[getModuleAddress()] = my_pos;
	
	return true;
}

/*!
 * 	@brief Provide the current position of the node to the subsribed modules.
 */

void Sunset_Position::sendPosition(int node)
{
	notified_info ni;
	
	if (sid == NULL) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "  Sunset_Position::sendPosition DISPATCHER NOT DEFINED");
		
		return;
	}
	
	if (position_info.find(node) == position_info.end()) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Position::sendPosition no info node %d ERROR", node);
		
		node_position aux;
		aux.latitude = 0.0;
		aux.longitude = 0.0;
		aux.depth = 0.0;
		position_info[node] = aux;
	}

	
	ni.node_id = node;
	ni.info_time = NOW;
	ni.info_name = "NODE_POSITION";
	
	node_position pos = position_info[node];

	if ( sid->assign_value(&pos, &ni, sizeof(node_position)) == false) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Position::sendPosition ERROR ASSIGINING INFO %s", (ni.info_name).c_str());
		
		return;
		
	} 
	
	if ( sid->set(getModuleAddress(), sid_id, ni) == 0 ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Position::sendPosition PROVIDING INFO %s NOT DEFINED", (ni.info_name).c_str());
		
	}
	
	Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Position::sendPosition Node %d Position: %f %f %f", node, pos.latitude, pos.longitude, pos.depth);
	
	return;
}

