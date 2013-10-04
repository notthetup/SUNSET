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


#include <sunset_queue.h>
#include <sunset_debug.h>

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_QueueClass : public TclClass 
{	
public:
	Sunset_QueueClass() : TclClass("Queue/Sunset_Queue") {}
	
	TclObject* create(int, const char*const*) {
		return (new Sunset_Queue());
	}
	
} class_Sunset_Queue;

/*! @brief This constructor sets up the queue parameters and creates a new PacketQueue object. */

Sunset_Queue::Sunset_Queue() : TclObject() 
{
	
	// Get variables initialization from the Tcl script
	
	bind_bool("drop_front_", &drop_front_);
	bind_bool("queue_in_bytes_", &qib_);
	bind("qlimBytes", &qlimBytes);
	bind("limit_", &qlim_);
	bind("moduleAddress", &module_address);
	
	pq_ = new PacketQueue();
	
	stat = NULL;
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Queue limit %d dropFront %d qib %d", qlim_, drop_front_, qib_);
}

/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script 
 *	@param[in] argc argc is a count of the arguments supplied to the command function.
 *	@param[in] argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_Queue::command(int argc, const char*const* argv) 
{
	
	Tcl& tcl = Tcl::instance();
	
	if (argc == 2) {
		
		/* The "start" command starts the queue module */
		
		if (strcmp(argv[1], "start") == 0) {
			
			Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_Agent::command start Module");
			start();
			
			return TCL_OK;
		}
		
		/* The "stop" command stops the queue module */
		
		if (strcmp(argv[1], "stop") == 0) {
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Agent::command stop Module");
			stop();
			
			return TCL_OK;
		}
	}
	else if ( argc == 3 ) {
		
		/* The "setModuleAddress" command sets the address of the queue module. */
		
		if (strcmp(argv[1], "setModuleAddress") == 0) {
			
			module_address = atoi(argv[2]);
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Queue::command macAddress %d", getModuleAddress());
			
			return (TCL_OK);
		}
		
	}
	
	return TclObject::command(argc, argv);
}

/*!
 * 	@brief The start() function can be called from the TCL script to execute Queue module operations when the simulation/emulation starts.
 */

void Sunset_Queue::start()
{
	Sunset_Module::start();
	
	if (Sunset_Statistics::use_stat()) {
		
		stat = Sunset_Statistics::instance();   
	}
	
	return;
}

/*!
 * 	@brief The stop() function can be called from the TCL script to execute Queue module operations when the simulation/emulation stops.
 */

void Sunset_Queue::stop()
{
	Sunset_Module::stop();
	
	stat = NULL;
	
	return;
}

/*! @brief This function inserts a packet element at the beginning of the queue.
 *  @param[in] p The packet to be enqueued.
 */

void Sunset_Queue::enqueFront(Packet* p)
{
	
	if (Sunset_Statistics::use_stat() && stat != NULL) {
		
		stat->logStatInfo(SUNSET_STAT_ENQUE, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");	
		
	}
	
	if ( ((pq_->length() + 1) > qlim_) ||
	    (qib_ && (pq_->byteLength() + hdr_cmn::access(p)->size()) > qlimBytes) ){
		
		if (Sunset_Statistics::use_stat() && stat != NULL) {
			
			stat->logStatInfo(SUNSET_STAT_QUEUE_DISCARD, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");	
		}
		
		if ( drop_front_ ) { /* remove from head of queue */
			
			Packet *pp = pq_->deque();
			pq_->enqueHead(p);
			
			Sunset_Utilities::erasePkt(pp, getModuleAddress());
			
		} else {
			
			Sunset_Utilities::erasePkt(p, getModuleAddress());
		}
		
		Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Queue::enqueFront DISCARDING PKT - queueLength %d size %d", length(), byteLength());
		
	} else {
		
		Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Queue::enqueFront ENQUE - queueLength %d size %d", length(), byteLength());
		
		pq_->enqueHead(p);
	}
	
	return;
}

/*! @brief This function inserts a packet at the end of the queue.
 *  @param[in] p The packet to be enqueued.
 */

void Sunset_Queue::enque(Packet* p)
{
	
	if (Sunset_Statistics::use_stat() && stat != NULL) {
		
		stat->logStatInfo(SUNSET_STAT_ENQUE, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");
		
	}
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Queue::enque  queueLength %d", length());
	
	if ( ((pq_->length() + 1) > qlim_) ||
	    (qib_ && (pq_->byteLength() + hdr_cmn::access(p)->size()) > qlimBytes) ) {
		
		/* the queue would overflow if we added this packet... */
		
		if (Sunset_Statistics::use_stat() && stat != NULL) {
			
			stat->logStatInfo(SUNSET_STAT_QUEUE_DISCARD, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");	
		}
		
		if (drop_front_) { /* remove from head of queue */
			
			pq_->enque(p);
			Packet *pp = pq_->deque();
			
			Sunset_Utilities::erasePkt(pp, getModuleAddress());
			
		} else {
			
			Sunset_Utilities::erasePkt(p, getModuleAddress());
		}
		
		Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Queue::enque DISCARDING PKT - queueLength %d size %d", length(), byteLength());
		
	} else {
		
		Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Queue::enque ENQUE PKT - queueLength %d size %d", length(), byteLength());
		
		pq_->enque(p);
	}
	
	return;
}

/*! @brief This function returns the first packet in the queue: the packet will be removed from the queue.
 *  @retval p The packet to be dequeued.
 */

Packet* Sunset_Queue::deque()
{
	Packet* p = pq_->deque();
	
	if (Sunset_Statistics::use_stat() && stat != NULL) {
		
		stat->logStatInfo(SUNSET_STAT_DEQUE, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");	
	}
	
	return p;
}

/*! @brief This function removes a specific packet which must be in the queue.
 *  @param[in] p The packet to be removed.
 */

void Sunset_Queue::remove(Packet* p) 
{
	if (Sunset_Statistics::use_stat() && stat != NULL) {
		
		stat->logStatInfo(SUNSET_STAT_DEQUE, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");	
	}
	
	pq_->remove(p);
	
	return;
}

/*! @brief This function removes a specific packet which must be in the queue.
 *  @param[in] p The packet to be removed.
 */

void Sunset_Queue::reset() 
{
	if (pq_->length() > 1) {
		
		Packet *pp = pq_->deque();
		Sunset_Utilities::erasePkt(pp, getModuleAddress());
		reset();
	}
	
	return;
}
