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

/*
 * The author would like to thank Daniele Spaccini <spaccini@di.uniroma1.it>, which has contributed to the development of this module. 
 */

#include <sunset_utilities.h>
#include <sunset_information_dispatcher.h>
#include <sunset_debug.h>

Sunset_Information_Dispatcher* Sunset_Information_Dispatcher::instance_ = NULL;

// TIMER
void Sunset_Information_Dispatcher_Timer::start(double time, int node, string param) 
{
	Scheduler& s = Scheduler::instance();
	
	node_id = node;
	
	parameter = param;
	
	if (busy_) {
		
		Sunset_Debug::debugInfo(-1, node, "INFORMATION_DISPATCHER Start timer busy ERROR");
		return;
	} 
	
	busy_ = 1;
	paused_ = 0;
	s.sync();
	stime = s.clock();
	rtime = time;
	
	if (rtime < 0.0) {
		Sunset_Debug::debugInfo(-1, node, "INFORMATION_DISPATCHER Start timer wrong time (%f) ERROR", rtime);
		return;	
	}
	
	Sunset_Debug::debugInfo(3, node, "INFORMATION_DISPATCHER Start timer");
	Sunset_Utilities::schedule(this, &intr, rtime);
}

void Sunset_Information_Dispatcher_Timer::stop(void) 
{
	Scheduler& s = Scheduler::instance();

	if(paused_ == 0) {
		
		s.cancel(&intr);
	}
	
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;
	
	node_id = -1;
	parameter = "";
}


void Sunset_Information_Dispatcher_Timer::handle(Event *e) 
{
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;
	
	Sunset_Debug::debugInfo(3, node_id, "INFORMATION_DISPATCHER Hanlde timer node %d parameter %s",
				node_id, parameter.c_str());
	
	node_id = -1;
	parameter = "";
}

///////////////////////////////////////////////////////////////////

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_Information_DispatcherTClass : public TclClass 
{
public:
	Sunset_Information_DispatcherTClass() : TclClass("Module/Sunset_Information_Dispatcher") {}
	
	TclObject* create(int argc , const char*const* argv) {
		
		return new Sunset_Information_Dispatcher();
	}
	
} class_Sunset_Information_Dispatcher;


Sunset_Information_Dispatcher::Sunset_Information_Dispatcher(): timer(this) 
{
	instance_ = this;
	module_counter = 1;
	
	Sunset_Debug::debugInfo(2, -1, "Sunset_Information_Dispatcher::Sunset_Information_Dispatcher CREATED");
}

Sunset_Information_Dispatcher::~Sunset_Information_Dispatcher() 
{
	instance_ = NULL;

	map<int, std::set<string> >::iterator it;
	
	for ( it = parameters.begin(); it != parameters.end(); it++ ) {
		
		parameters.erase(it);
	}
	
	parameters.clear();
}

/*!
 * 	@brief The stop() function can be called from the TCL script to execute information dispatcher operations when the simulation/emulation stops.
 */

void Sunset_Information_Dispatcher::stop() 
{
}

/*!
 * 	@brief The start() function can be called from the TCL script to execute information dispatcher operations when the simulation/emulation starts.
 */

void Sunset_Information_Dispatcher::start() 
{
	Sunset_Debug::debugInfo(2, -1, "Sunset_Information_Dispatcher::starting the module");
}

/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script 
 *	@param[in] argc argc is a count of the arguments supplied to the command function.
 *	@param[in] argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_Information_Dispatcher::command(int argc, const char*const* argv) 
{
	Tcl& tcl = Tcl::instance();
	
	if (argc == 2) {
		
		/* The "start" command starts the Information Dispatcher module operations */
		
		if (strcasecmp(argv[1], "start") == 0) {
			
			Sunset_Debug::debugInfo(3, -1, "Sunset_Information_Dispatcher::command start Module");
			start();
			
			return TCL_OK;
		}
		
		/* The "stop" command stops the Information Dispatcher module operations */
		
		if (strcasecmp(argv[1], "stop") == 0) {
			
			Sunset_Debug::debugInfo(3, -1, "Sunset_Information_Dispatcher::command stop Module");
			stop();
			
			return TCL_OK;
		}
	}
	else if ( argc == 4 ) {
		
	  	/* The "addParameter" command adds a parameter to the information dispatcher that can be requested or provided by the registered modules */
		
		if (strcasecmp(argv[1], "addParameter") == 0) {
			
			int my_id = atoi(argv[2]);
			string tmp(argv[3]);
			Sunset_Utilities::toUpperString(tmp);
			
			parameters[my_id].insert(tmp);
			
			Sunset_Debug::debugInfo(3, my_id, "Sunset_Information_Dispatcher::command paramter added %s", argv[3]);
			
			return TCL_OK;
		}
		
	  	/* The "register_module" command regiters a module to the information dispatcher */
		
		if (strcasecmp(argv[1], "register_module") == 0) 
		{
			int my_id = atoi(argv[2]);
			int module_id = register_module(my_id, "-", (Sunset_Dispatched_Module*) 
			
			TclObject::lookup(argv[3]));
			
			Sunset_Debug::debugInfo(3, my_id, "Sunset_Information_Dispatcher::command module registered module_id %d", module_id);	
			
			tcl.resultf("%f",module_id);
			
			return TCL_OK;
			
		}
	}
	else if ( argc == 5 ) {
		
	  	/* The "register_module" command regiters a module to the information dispatcher providing also a description string for the module */
		
		if (strcasecmp(argv[1], "register_module") == 0) {
			
			int my_id = atoi(argv[2]);
			int module_id = register_module(my_id, argv[4], (Sunset_Dispatched_Module*) 
			
			TclObject::lookup(argv[3]));
			
			Sunset_Debug::debugInfo(3, my_id, "Sunset_Information_Dispatcher::command module registered module_id %d name %s", module_id, argv[4]);	
			
			tcl.resultf("%f",module_id);
			
			return TCL_OK;
			
		}
	}
	
	return TclObject::command(argc, argv);
}

/*!
 * 	@brief The register_module() function registers the module in input to the information dispatcher using the provided string as a descritpion for the module.
 *	@param[in] my_id ID of the node executing the registration.
 *	@param[in] s Description string of the module in input.
 *	@param[in] sm Reference to the module registering to the information dispatcher.
 *	@retval module_id ID assigned by the information dispatcher to the register module. 
 */

int Sunset_Information_Dispatcher::register_module(int my_id, string s, Sunset_Dispatched_Module* sm) 
{
	
	registered_module rm;
	
	rm.module = sm;
	rm.name = s;
	rm.module_id = module_counter;
	
	(moduleMap[my_id])[module_counter++] = rm;
	
	Sunset_Debug::debugInfo(3, my_id, "Sunset_Information_Dispatcher::register_module module registered module_id %d name %s", rm.module_id, (rm.name).c_str());
	
	return rm.module_id;
	
}

/*!
 * 	@brief The register_module() function registers the module in input to the information dispatcher.
 *	@param[in] my_id ID of the node executing the registration.
 *	@param[in] sm Reference to the module registering to the information dispatcher.
 *	@retval module_id ID assigned by the information dispatcher to the register module. 
 */

int Sunset_Information_Dispatcher::register_module (int my_id, Sunset_Dispatched_Module* sm) 
{
	
	registered_module rm;
	
	rm.module = sm;
	rm.name = "-"; // a dummy description string is assigned to the module
	rm.module_id = module_counter;
	
	(moduleMap[my_id])[module_counter++] = rm;
	
	Sunset_Debug::debugInfo(3, my_id, "Sunset_Information_Dispatcher::register_module module registered module_id %d", rm.module_id);	
	
	return rm.module_id;
	
}

/*!
 * 	@brief The set() function notifies to the dispatcher that a new information has been provided by module with ID module_id.
 *	@param[in] my_id ID of the node executing the operation.
 *	@param[in] module_id ID of the module providing the information (this is the one assigned by the information dispatcher during the registration phase).
 *	@param[in] ni Provided information.
 *	@retval 1 Operation correctly completed. 
 *	@retval 0 Error. 
 */

int Sunset_Information_Dispatcher::set(int my_id, int module_id, notified_info ni) 
{
	// check if ni is correct and the requested information is provided by the dispatcher with a correct module_id
	
	if (check(my_id, module_id, ni.info_name) == 0) {
		
		Sunset_Debug::debugInfo(-1, my_id, "Sunset_Information_Dispatcher::set module_id %d INFO %s NOT DEFINED", module_id, (ni.info_name).c_str());
		
		// ERROR
		return 0;
	}
	
	// check if node_id has signed to provide this kind of information
	
	if (provided_info.find(my_id) == provided_info.end()) {
		
		Sunset_Debug::debugInfo(-1, my_id, "Sunset_Information_Dispatcher::set node not registered to provide this  info %s", (ni.info_name).c_str());	
		
		// ERROR
		return 0;
	}
	
	// check if module_id has signed to provide this kind of information
	
	if (provided_info[my_id].find(ni.info_name) == provided_info[my_id].end() ||
	    ((provided_info[my_id])[ni.info_name]).find(module_id) == ((provided_info[my_id])[ni.info_name]).end()) {
		
		Sunset_Debug::debugInfo(-1, my_id, "Sunset_Information_Dispatcher::set module %d not registered to provide this  info %s", module_id, (ni.info_name).c_str());	
		
		// ERROR
		return 0;
	}
	
	// check if an older information was stored and free the allocated memory
	
	if ( node_info.find(my_id) != node_info.end() &&
	    node_info[my_id].find(ni.info_name) != node_info[my_id].end() && 
	    (node_info[my_id])[ni.info_name].find(ni.node_id) != (node_info[my_id])[ni.info_name].end()) {
		
		if ((((node_info[my_id])[ni.info_name])[ni.node_id]).info_size != 0 ) {
			
			free((((node_info[my_id])[ni.info_name])[ni.node_id]).info_value);			      
		}
		
	}
	
	// store the provided information
	
	(((node_info[my_id])[ni.info_name])[ni.node_id]).info_size = ni.info_size;

	(((node_info[my_id])[ni.info_name])[ni.node_id]).info_value = ni.info_value;
	
	(((node_info[my_id])[ni.info_name])[ni.node_id]).info_time = ni.info_time;
	
	Sunset_Debug::debugInfo(3, my_id, "Sunset_Information_Dispatcher::set module_id %d node_id %d name %s time %f", module_id, ni.node_id, (ni.info_name).c_str(), ni.info_time);
	
	// check if some modules have to be notified about the updated information
	if (subscribed_info.find(my_id) != subscribed_info.end() &&
	    (subscribed_info[my_id]).find(ni.info_name) != (subscribed_info[my_id]).end()) {
		
		std::set<int>::iterator it = ((subscribed_info[my_id])[ni.info_name]).begin();
		list<notified_info> linfo;
		
		linfo.push_back(ni);
		
		Sunset_Debug::debugInfo(3, my_id, "Sunset_Information_Dispatcher::set modules subscribed for %s size %d",  (ni.info_name).c_str(), ((subscribed_info[my_id])[ni.info_name]).size());
		
		for (; it != ((subscribed_info[my_id])[ni.info_name]).end(); it++) {
			
			if ( moduleMap.find(my_id) != moduleMap.end() &&
			    (moduleMap[my_id]).find(*it) != (moduleMap[my_id]).end()) {
				
				registered_module rm = (moduleMap[my_id])[*it];
				
				if (rm.module_id != module_id) {
					
					Sunset_Debug::debugInfo(3, my_id, "Sunset_Information_Dispatcher::set notify to module_id %d name %s", (rm).module_id, (rm.name).c_str());
					
					// notify this information to the subscribed module 
					
					(rm.module)->notify_info(linfo);			
				}	
				else {
					Sunset_Debug::debugInfo(3, my_id, "Sunset_Information_Dispatcher::set notify to module_id %d name %s NO FORWARD", (rm).module_id, (rm.name).c_str());
				}
			}
		}
	} 
	else {
		
		Sunset_Debug::debugInfo(3, my_id, "Sunset_Information_Dispatcher::set no module has subscribed %s", (ni.info_name).c_str());
	}
	
	// OK
	return 1;
}

/*!
 * 	@brief The get() function collects from the dispatcher the latest stored information for a given parameter.
 *	@param[in] my_id ID of the node executing the operation.
 *	@param[in] module_id ID of the module requesting the information (this is the one assigned by the information dispatcher during the registration phase).
 *	@param[in] parameter The parameter the module is interesting in.
 *	@param[in] ni List of notified info were to store the requested information.
 *	@retval 1 Operation correctly completed. 
 *	@retval 0 Error. 
 */

int Sunset_Information_Dispatcher::get(int my_id, int module_id, string parameter, list<notified_info>& list_ni)
{
	map<int, stored_info >::iterator it;
	
	//check if the requested parameter is provided by the dispatcher and the module_id is correct
	if (check(my_id, module_id, parameter) == 0) {
		
		Sunset_Debug::debugInfo(0, my_id, "Sunset_Information_Dispatcher::get module_id %d INFO %s NOT DEFINED", module_id, (parameter).c_str());
		
		// ERROR	
		return 0;
		
	}
	
	//check if the node_id has signed to request this kind of information
	if (subscribed_info.find(my_id) == subscribed_info.end()) {
		
		Sunset_Debug::debugInfo(-1, my_id, "Sunset_Information_Dispatcher::get node not registered to request this  info %s", (parameter).c_str());	
		
		// ERROR
		return 0;
	}
	
	//check if the module_id has signed to request this kind of information
	if ((subscribed_info[my_id]).find(parameter) == (subscribed_info[my_id]).end() || 
	    ((subscribed_info[my_id])[parameter]).find(module_id) == ((subscribed_info[my_id])[parameter]).end()) {
		Sunset_Debug::debugInfo(-1, my_id, "Sunset_Information_Dispatcher::get module %d not registered to request this  info %s", module_id, (parameter).c_str());
		
		// ERROR
		return 0;
	}
	
	//check if the dispatcher has the requested information
	if ( node_info.find(my_id) == node_info.end() || node_info[my_id].find(parameter) == node_info[my_id].end()) {
		
		Sunset_Debug::debugInfo(-1, my_id, "Sunset_Information_Dispatcher::get module_id %d INFO %s VAL NOT DEFINED", module_id, (parameter).c_str());
		
		//ERROR
		return 0;
		
	}
	
	it = ((node_info[my_id])[parameter]).begin();
	
	//collect the requested information for all the different node_id (if more than one)
	for (; it != ((node_info[my_id])[parameter]).end(); it++) {
		
		notified_info ni;
		ni.info_name = parameter;
		ni.node_id = it->first;
		ni.info_time = (it->second).info_time;
		ni.info_value = (it->second).info_value;
		ni.info_size = (it->second).info_size;
		list_ni.push_back(ni);
	}
	
	// OK
	return 1;
}

/*!
 * 	@brief The get() function collects from the dispatcher the latest stored information for a given parameter provided by a given node.
 *	@param[in] my_id ID of the node executing the operation.
 *	@param[in] module_id ID of the module requesting the information (this is the one assigned by the information dispatcher during the registration phase).
 *	@param[in] parameter The parameter the module is interesting in.
 *	@param[in] node The ID of the node providing the information the module is interesting in.
 *	@param[in] ni Notified info were to store the requested information.
 *	@retval 1 Operation correctly completed. 
 *	@retval 0 Error. 
 */

int Sunset_Information_Dispatcher::get(int my_id, int module_id, string parameter, int node, notified_info& ni)
{
	//check if the requested parameter is provided by the dispatcher and the module_id is correct
	if (check(my_id, module_id, parameter) == 0) {
		
		Sunset_Debug::debugInfo(-1, my_id, "Sunset_Information_Dispatcher::get module_id %d node %d INFO %s NOT DEFINED", module_id, node, (parameter).c_str());
		
		//ERROR	
		return 0;
		
	}
	
	//check if the node_id has signed to request this kind of information
	if (subscribed_info.find(my_id) == subscribed_info.end()) {
		
		Sunset_Debug::debugInfo(-1, my_id, "Sunset_Information_Dispatcher::get node not registered to request this  info %s", (parameter).c_str());	
		
		// ERROR
		return 0;
	}
	
	//check if the module_id has signed to request this kind of information
	if ((subscribed_info[my_id]).find(parameter) == (subscribed_info[my_id]).end() || 
	    ((subscribed_info[my_id])[parameter]).find(module_id) == ((subscribed_info[my_id])[parameter]).end()) {
		
		Sunset_Debug::debugInfo(-1, my_id, "Sunset_Information_Dispatcher::get module %d not registered to request this  info %s", module_id, (parameter).c_str());
		
		// ERROR
		return 0;
	}
	
	//check if the dispatcher has the requested information
	if ( node_info.find(my_id) == node_info.end() || node_info[my_id].find(parameter) == node_info[my_id].end()) {
		
		Sunset_Debug::debugInfo(-1, my_id, "Sunset_Information_Dispatcher::get module_id %d INFO %s VAL NOT DEFINED", module_id, (parameter).c_str());
		
		//ERROR
		return 0;
		
	}
	
	
	//collect the requested information
	ni.info_name = parameter;
	ni.node_id = node;
	ni.info_time = ((node_info[my_id])[parameter])[node].info_time;
	ni.info_value = ((node_info[my_id])[parameter])[node].info_value;
	ni.info_size = ((node_info[my_id])[parameter])[node].info_size;
	
	// OK
	return 1;
}

/*!
 * 	@brief The provide() function notifies the information dispatcher that the module with ID module_id will provide information of parameter type.
 *	@param[in] my_id ID of the node executing the operation.
 *	@param[in] module_id ID of the module providing the information (this is the one assigned by the information dispatcher during the registration phase).
 *	@param[in] parameter The parameter the module is providing.
 *	@retval 1 Operation correctly completed. 
 *	@retval 0 Error. 
 */

int Sunset_Information_Dispatcher::provide(int my_id, int module_id, string parameter)
{
	// check if node_id is a correct one assigned by the dispatcher
	if ( moduleMap.find(my_id) == moduleMap.end()) {
		
  		Sunset_Debug::debugInfo(-1, my_id, "Sunset_Information_Dispatcher::provide node name %s MODULE NOT PRESENT", module_id, parameter.c_str());	
		
		//ERROR
		return 0;
		
	}
	
	// check if module_id is a correct one assigned by the dispatcher
	if ( moduleMap[my_id].find(module_id) == moduleMap[my_id].end()) {
		
  		Sunset_Debug::debugInfo(-1, my_id, "Sunset_Information_Dispatcher::provide module_id %d name %s MODULE NOT PRESENT", module_id, parameter.c_str());	
		
		//ERROR
		return 0;
		
	}
	
	// check if parameter is known by the dispatcher
	if (parameters.find(my_id) == parameters.end() || parameters[my_id].find(parameter) == parameters[my_id].end()) {
		
		Sunset_Debug::debugInfo(-1, my_id, "Sunset_Information_Dispatcher::provide module_id %d name %s NOT PRESENT", module_id, parameter.c_str());	
		
		printAddedParameters(my_id);
		
		// ERROR
		return 0;
	}
	
	
	Sunset_Debug::debugInfo(3, my_id, "Sunset_Information_Dispatcher::provide module_id %d name %s ADDED", module_id, parameter.c_str());	
	
	// register the provided information
	((provided_info[my_id])[parameter]).insert(module_id);
	
	//OK
	return 1;
}

/*!
 * 	@brief The check() function controls if the parameter in input is supported by the dispatcher.
 *	@param[in] my_id ID of the node executing the operation.
 *	@param[in] module_id ID of the module requesting to execute an action (set/get).
 *	@param[in] parameter The parameter the module is interested in.
 *	@retval 1 Operation correctly completed. 
 *	@retval 0 Error. 
 */

int Sunset_Information_Dispatcher::check(int my_id, int module_id, string parameter)
{
	Sunset_Debug::debugInfo(3, my_id, "Sunset_Information_Dispatcher::check module_id %d name %s", module_id, parameter.c_str());
	
	// check if the requested parameter is known by the dispatcher for this node
	if (parameters.find(my_id) == parameters.end() || parameters[my_id].find(parameter) == parameters[my_id].end()) {
		
		Sunset_Debug::debugInfo(-1, my_id, "Sunset_Information_Dispatcher::check name %s NOT PRESENT", 
					parameter.c_str());	
		
		printAddedParameters(my_id);
		
		// ERROR
		return 0;
	}
	
	// check if the node_id and module_id are known by the dispatcher
	if ( moduleMap.find(my_id) == moduleMap.end() || moduleMap[my_id].find(module_id) == moduleMap[my_id].end()) {
		
  		Sunset_Debug::debugInfo(-1, my_id, "Sunset_Information_Dispatcher::check module_id %d NOT PRESENT", module_id);	
		
		//ERROR
		return 0;
		
	}

	// check if the parameter is provided by the node requesting it.
	if ( is_provided( my_id, module_id, parameter ) == 0) {
		
  		Sunset_Debug::debugInfo(-1, my_id, "Sunset_Information_Dispatcher::check parameter %s not provieded by the node %d requesting it", parameter.c_str(), my_id);
		
		//ERROR
		return 0;
		
	}
	
	Sunset_Debug::debugInfo(3, my_id, "Sunset_Information_Dispatcher::check module_id %d name %s OK", module_id, parameter.c_str());	
	
	// OK
	return 1;
}

/*!
 * 	@brief The define() function defines a new parameter for the dispatcher.
 *	@param[in] my_id ID of the node executing the operation.
 *	@param[in] module_id ID of the module defining the new parameter.
 *	@param[in] parameter The parameter the module is defining.
 *	@retval 1 Operation correctly completed. 
 *	@retval 0 Error. 
 */

int Sunset_Information_Dispatcher::define(int my_id, int module_id, string new_parameter)
{
	// check if the parameter is already known by the dispatcher. If this is the case it does not have to be defined
	if (parameters.find(my_id) != parameters.end() && parameters[my_id].find(new_parameter) != parameters[my_id].end()) {
		
		Sunset_Debug::debugInfo(-1, my_id, "Sunset_Information_Dispatcher::define module_id %d INFO %s ALREADY DEFINED", module_id, (new_parameter).c_str());
		
		printAddedParameters(my_id);
		
		// ERROR
		return 0;
	}
	
	// ADD the parameter
	parameters[my_id].insert(new_parameter);
	
	//OK
	return 1;
}

/*!
 * 	@brief The printAddedParameters() function prints out the parameter known by the information dispatcher.
 *	@param[in] my_id ID of the node executing the operation.
 */

void Sunset_Information_Dispatcher::printAddedParameters(int my_id) 
{
	std::set<string>::iterator it;
	
	if (parameters.find(my_id) == parameters.end()) {
		
		return;
		
	}
	
	it = parameters[my_id].begin();
	
	for (int i = 1; it != parameters[my_id].end(); it++, i++) {
		
		Sunset_Debug::debugInfo(5, my_id, "Sunset_Information_Dispatcher::printAddedParameters id %d name %s", i, (*it).c_str());	
	}
}

/*!
 * 	@brief The is_provided() function checks if the information about a given parameter are provided by the dispatcher.
 *	@param[in] my_id ID of the node executing the operation.
 *	@param[in] module_id ID of the module making the request.
 *	@param[in] parameter The parameter the module is interesting in.
 *	@retval 1 Operation correctly completed. 
 *	@retval 0 Error. 
 */

int Sunset_Information_Dispatcher::is_provided(int my_id, int module_id, string parameter)
{
	Sunset_Debug::debugInfo(3, my_id, "Sunset_Information_Dispatcher::is_provided module_id %d name %s", module_id, parameter.c_str());	
	
	// check if the requested parameter is provided by the dispatcher
	if (provided_info.find(my_id) == provided_info.end() || 
	    provided_info[my_id].find(parameter) == provided_info[my_id].end() || 
	    ((provided_info[my_id])[parameter]).empty()) {
		
		Sunset_Debug::debugInfo(-1, my_id, "Sunset_Information_Dispatcher::is_provided module_id %d name %s NOT PRESENT", module_id, parameter.c_str());	
		
		printAddedParameters(my_id);
		
		//ERROR
		return 0;
	}
	
	Sunset_Debug::debugInfo(3, my_id, "Sunset_Information_Dispatcher::check is_provided %d name %s OK", module_id, parameter.c_str());	
	
	// OK
	return 1;
}

/*!
 * 	@brief The stop_providing() function notifies the information dispatcher that the given module will not provide parameter information anymore.
 *	@param[in] my_id ID of the node executing the operation.
 *	@param[in] module_id ID of the module making the request.
 *	@param[in] parameter The parameter the module is interesting in.
 *	@retval 1 Operation correctly completed. 
 *	@retval 0 Error. 
 */

int Sunset_Information_Dispatcher::stop_providing(int my_id, int module_id, string parameter)
{
	Sunset_Debug::debugInfo(3, my_id, "Sunset_Information_Dispatcher::stop_providing module_id %d name %s", module_id, parameter.c_str());	
	
	// check if the requested parameter is provided by the dispatcher
	if (provided_info.find(my_id) == provided_info.end() || 
	    provided_info[my_id].find(parameter) == provided_info[my_id].end()) {
		
		Sunset_Debug::debugInfo(-1, my_id, "Sunset_Information_Dispatcher::stop_providing module_id %d name %s NOT PRESENT", module_id, parameter.c_str());	
		
		printAddedParameters(my_id);
		
		//ERROR
		return 0;
	}
	
	
	// check if the module was providing the given parameter
	if (((provided_info[my_id])[parameter]).find(module_id) == ((provided_info[my_id])[parameter]).end()) {
		
		Sunset_Debug::debugInfo(-1, my_id, "Sunset_Information_Dispatcher::stop_providing module_id %d name %s NOT PRESENT", module_id, parameter.c_str());	
		
		printAddedParameters(my_id);
		
		//ERROR
		return 0;
	}
	
	// remove the information from the dispatcher
	((provided_info[my_id])[parameter]).erase(((provided_info[my_id])[parameter]).find(module_id));
	
	if ( ((provided_info[my_id])[parameter]).empty() ) {
		
		((provided_info[my_id])).erase((provided_info[my_id]).find(parameter));
	}
	
	Sunset_Debug::debugInfo(3, my_id, "Sunset_Information_Dispatcher::stop_providing is_provided %d name %s REMOVED", module_id, parameter.c_str());	
	
	// OK
	return 1;
}

/*!
 * 	@brief The remove_subscription() function notifies the information dispatcher that the given module does not want to be informed about parameter information anymore.
 *	@param[in] my_id ID of the node executing the operation.
 *	@param[in] module_id ID of the module making the request.
 *	@param[in] parameter The parameter the module is interesting in.
 *	@retval 1 Operation correctly completed. 
 *	@retval 0 Error. 
 */

int Sunset_Information_Dispatcher::remove_subscription(int my_id, int module_id, string parameter)
{
	Sunset_Debug::debugInfo(3, my_id, "Sunset_Information_Dispatcher::remove_subscription module_id %d name %s", module_id, parameter.c_str());	
	
	// check if the requested parameter is provided by the dispatcher
	if (subscribed_info.find(my_id) == subscribed_info.end() || 
	    subscribed_info[my_id].find(parameter) == subscribed_info[my_id].end()) {
		
		Sunset_Debug::debugInfo(-1, my_id, "Sunset_Information_Dispatcher::remove_subscription module_id %d name %s NOT PRESENT", module_id, parameter.c_str());	
		
		//ERROR
		return 0;
	}
	
	// check if the module was signed for the given parameter
	if (((subscribed_info[my_id])[parameter]).find(module_id) == ((subscribed_info[my_id])[parameter]).end()) {
		Sunset_Debug::debugInfo(-1, my_id, "Sunset_Information_Dispatcher::remove_subscription module_id %d name %s NOT PRESENT", module_id, parameter.c_str());	
		
		//ERROR
		return 0;
	}
	
	// remove the information from the dispatcher
	((subscribed_info[my_id])[parameter]).erase(((subscribed_info[my_id])[parameter]).find(module_id));
	
	if ( ((subscribed_info[my_id])[parameter]).empty() ) {
		
		((subscribed_info[my_id])).erase((subscribed_info[my_id]).find(parameter));
	}
	
	Sunset_Debug::debugInfo(3, my_id, "Sunset_Information_Dispatcher::remove_subscription  %d name %s REMOVED", module_id, parameter.c_str());	
	
	return 1;
}

/*!
 * 	@brief The subscribe() function notifies the information dispatcher that the given module wants to be informed about parameter information.
 *	@param[in] my_id ID of the node executing the operation.
 *	@param[in] module_id ID of the module making the request.
 *	@param[in] parameter The parameter the module is interesting in.
 *	@retval 1 Operation correctly completed. 
 *	@retval 0 Error. 
 */

int Sunset_Information_Dispatcher::subscribe(int my_id, int module_id, string parameter) 
{
	// check if the node_id and module_id are known by the dispatcher and the module_id is correct
	if ( moduleMap.find(my_id) == moduleMap.end() || moduleMap[my_id].find(module_id) == moduleMap[my_id].end()) {
		
  		Sunset_Debug::debugInfo(-1, my_id, "Sunset_Information_Dispatcher::subscribe module_id %d name %s MODULE NOT PRESENT", module_id, parameter.c_str());	
		
		//ERROR
		return 0;
		
	}
	
	// check if the requested parameter is known by the dispatcher
	if (parameters.find(my_id) == parameters.end() || parameters[my_id].find(parameter) == parameters[my_id].end()) {
		
		Sunset_Debug::debugInfo(-1, my_id, "Sunset_Information_Dispatcher::subscribe module_id %d name %s NOT PRESENT", module_id, parameter.c_str());	
		
		printAddedParameters(my_id);
		
		//ERROR
		return 0;
	}
	
	// register the module request
	((subscribed_info[my_id])[parameter]).insert(module_id);
	
	Sunset_Debug::debugInfo(-1, my_id, "Sunset_Information_Dispatcher::subscribe module_id %d name %s", module_id, parameter.c_str());	
	
	//OK
	return 1;
}

