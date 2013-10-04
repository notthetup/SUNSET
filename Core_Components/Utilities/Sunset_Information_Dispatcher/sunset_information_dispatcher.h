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

#ifndef __Sunset_Information_Dispatcher_h__
#define __Sunset_Information_Dispatcher_h__

#include <iostream>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <fcntl.h>
#include <fstream>
#include <stdlib.h>
#include <map>
#include <list>
#include <set>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <module.h>
#include <node-core.h>
#include <sunset_debug.h>


class Sunset_Dispatched_Module;
class Sunset_Information_Dispatcher;

/*! @brief struct containing node position information. */
typedef struct node_position {
	
	double latitude;
	double longitude;
	double depth;
	
} node_position;

/*! @brief struct containing network link information. */
typedef struct link_info {
	
	int src;
	int dst;
	double value;
	
} link_info;

/*! @brief struct containing information related to the module registered to the information dispatcher. */
typedef struct registered_module {
	
	string name;				// module name
	
	int module_id;				// module id
	
	Sunset_Dispatched_Module* module;	// reference to teh module
	
} registered_module;

/*! @brief struct containing the information dispatched to the registered modules, according to the module interests. */
typedef struct notified_info {
	
	string info_name;	// information name
	
	int node_id;		// node ID which the information are related to (distance to, delayt to, etc.)
	
	void* info_value;	// information value
	
	size_t info_size;	// information size in bytes
	
	double info_time;	// information timestamp
	
} notified_info;

/*! @brief struct containing the information stored at the dispatcher. */
typedef struct stored_info {
	
	void* info_value;	// information value

	size_t info_size;	// information size in bytes

	double info_time;	// information timestamp
	
} stored_info;

class Sunset_Dispatched_Module {
	
public:
	
	Sunset_Dispatched_Module() { }
	
	// Each module registering to the Information Dispatcher has to implement this function to receive the dispatched information.
	virtual int notify_info(list<notified_info> linfo) = 0;
};

//===================================

/*! @brief This class implements the timer for the Information Dispatcher module. It can be used for periodic request to the dispatcher about some node parameters. */

class Sunset_Information_Dispatcher_Timer : public Handler {
public:
	Sunset_Information_Dispatcher_Timer(Sunset_Information_Dispatcher* info) : info_(info) {
		busy_ = paused_ = 0; stime = rtime = 0.0; node_id = -1; 
	}
	
	virtual void handle(Event *e);
	
	virtual void start(double time, int node, string param);
	
	virtual void stop(void);
	
	virtual void pause(void) { assert(0); }
	
	virtual void resume(void) { assert(0); }
	
	inline int busy(void) { return busy_; }
	
	inline int paused(void) { return paused_; }
	
	inline double expire(void) {
		
		Scheduler& s = Scheduler::instance();
		
		return ((stime + rtime) - s.clock());
	}
	
protected:
	
	Sunset_Information_Dispatcher *info_;
	int		busy_;
	int		paused_;
	Event		intr;
	double		stime;		// start time
	double		rtime;		// remaining time
	int node_id;    		// timer for node
	string parameter; 		// timer for parameter
};


/*! @brief This class implements the Information Dispatcher module. */

class Sunset_Information_Dispatcher: public TclObject {
	
	friend class  Sunset_Information_Dispatcher_Timer;
	
public:
	
	Sunset_Information_Dispatcher();
	~Sunset_Information_Dispatcher();
	
	virtual int command( int argc, const char*const* argv );
	
	static Sunset_Information_Dispatcher* instance() {
		
		if (instance_ == NULL) {
			
			Sunset_Information_Dispatcher();	
		}
		
		return (instance_);	// general access to information dispatcher object
	}
	
	/*!
	 * 	@brief The set() function notifies to the dispatcher that a new information has been provided by module with ID module_id.
	 */
	int set(int my_id, int module_id, notified_info ni);
	
	/*!
	 * 	@brief The get() function collects from the dispatcher the latest stored information for a given parameter.
	 */
	int get(int my_id, int module_id, string parameter, list<notified_info>& ni);
	
	/*!
	 * 	@brief The get() function collects from the dispatcher the latest stored information for a given parameter provided by a given node.
	 */
	int get(int my_id, int module_id, string parameter, int node, notified_info& ni);
	
	/*!
	 * 	@brief The provide() function notifies the information dispatcher that the module with ID module_id will provide information of parameter type.
	 */
	int provide(int my_id, int module_id, string parameter);
	
	/*!
	 * 	@brief The stop_providing() function notifies the information dispatcher that the given module will not provide parameter information anymore.
	 */
	int stop_providing(int my_id, int module_id, string parameter);
	
	/*!
	 * 	@brief The remove_subscription() function notifies the information dispatcher that the given module does not want to be informed about parameter information anymore.
	 */
	int remove_subscription(int my_id, int module_id, string parameter);
	
	/*!
	 * 	@brief The is_provided() function checks if the information about a given parameter are provided by the dispatcher.
	 */
	int is_provided(int my_id, int module_id, string parameter);
	
	/*!
	 * 	@brief The check() function controls if the parameter in input is supported by the dispatcher.
	 */
	int check(int my_id, int module_id, string parameter);
	
	/*!
	 * 	@brief The define() function defines a new parameter for the dispatcher.
	 */
	
	int define(int my_id, int module_id, string new_parameter);
	
	/*!
	 * 	@brief The register_module() function registers the module in input to the information dispatcher.
	 */
	int register_module (int my_id, Sunset_Dispatched_Module* sm);
	
	/*!
	 * 	@brief The register_module() function registers the module in input to the information dispatcher using the provided string as a descritpion for the module.
	 */
	int register_module (int my_id, string s, Sunset_Dispatched_Module* sm);
	
	/*!
	 * 	@brief The subscribe() function notifies the information dispatcher that the given module wants to be informed about parameter information.
	 */
	int subscribe(int my_id, int module_id, string parameter);
	
	// Functions defined to make easier the assignment and collection of information with different types and sizes
	template <typename T>
	bool get_value(T* val, notified_info info) {
		
		if (info.info_size == 0) {
			
			return false;
		}
		
		memcpy(val, info.info_value, info.info_size);
		
		return true;
	}
	
	template <typename T>
	bool get_value(T** val, notified_info info) {
		
		if (info.info_size == 0) {
			
			return false;
		}
		
		*val = (T *)malloc(info.info_size);
		
		if ( *val == NULL ) {
		
			Sunset_Debug::debugInfo(-1, -1, "Sunset_Information_Dispatcher::get_value MALLOC ERROR");
			
			return false;
		}
		
		memset(*val, 0x0, info.info_size);
		
		memcpy(*val, info.info_value, info.info_size);
		
		return true;
	}
	
	template <typename T>
	bool assign_value(T *val, notified_info* info, size_t size) {
		
		T* temp;
		
		if ( (int)size == 0 ) {
			
			return false;
		}
		
		temp = (T *)malloc(size);
		
		if ( temp == NULL ) {
		
			Sunset_Debug::debugInfo(-1, -1, "Sunset_Information_Dispatcher::assign_value MALLOC ERROR");
			
			return false;
		}
		
		memset(temp, 0x0, size);
		
		memcpy(temp, val, size);
		
		info->info_value = (void *) temp;
		info->info_size = size;
		
		return true;
	}
	
protected:
	
	Sunset_Information_Dispatcher_Timer timer;
	
	int dispatcher_address;
	
	/*!
	 * 	@brief The start() function can be called from the TCL script to execute information dispatcher operations when the simulation/emulation starts.
	 */
	void start();
	
	/*!
	 * 	@brief The stop() function can be called from the TCL script to execute information dispatcher operations when the simulation/emulation stops.
	 */
	void stop();
	
	/*!
	 * 	@brief The printAddedParameters() function prints out the parameter known by the information dispatcher.
	 */
	void printAddedParameters(int my_id);
	
	static Sunset_Information_Dispatcher* instance_;
	
	map<int, map < int, registered_module> > moduleMap; // registered modules
	
	map<int, map < string, map<int, stored_info > > > node_info; //it stores the node estimated value and timestamp of the information related to a given node and a parameter according to the pattern: <node_id, parameter_name, addressed_node, <value, time>> 
	
	map<int, std::set<string> > parameters; // registered parameters
	
	map<int, map< string, std::set<int> > > provided_info; // provided information <node_id, parameter, module_IDs>
	
	map<int, map<string, std::set<int> > > subscribed_info; // subscribed information <node_id, parameter, module_IDs>
	
	int module_counter;
	
};

#endif
