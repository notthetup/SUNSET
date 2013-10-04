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



#include "sunset_flooding.h"
#include <sunset_trace.h>
#include <address.h>

void Sunset_Flooding_Timer::start(double time) 
{
	Scheduler& s = Scheduler::instance();
	
	assert(busy_ == 0);
	
	if (busy()) {
		
		Sunset_Debug::debugInfo(-1, node_rtg->getModuleAddress(), "Sunset_Flooding_Timer::start timer busy ERROR");
		return;
	}
	
	busy_ = 1;
	paused_ = 0;
	
	s.sync();
	stime = s.clock();
	rtime = time;
	assert(rtime >= 0.0);
	
	Sunset_Utilities::schedule(this, &intr, rtime);
}

void Sunset_Flooding_Timer::stop(void) 
{
	Scheduler& s = Scheduler::instance();
	
	assert(busy_);
	
	if(paused_ == 0)
		s.cancel(&intr);
	
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;
}


void Sunset_Flooding_Timer::handle(Event *) 
{
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;
	
	node_rtg->sendPkt();
}


/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_FloodingClass : public TclClass 
{
	
public:
	Sunset_FloodingClass() : TclClass("Module/Sunset_Flooding") {}
	
	TclObject* create(int, const char*const*) {
		
		return (new Sunset_Flooding());
	}
	
} class_Sunset_FloodingClass;

Sunset_Flooding::Sunset_Flooding() : Sunset_Routing() 
{
	
	// Get variables initialization from the Tcl script
	bind("probability_", &probability_);
	bind("max_forwards_", &max_forwards_);
	bind("max_random_time_", &max_random_time_);
	
	floodingTimer_ = new Sunset_Flooding_Timer(this);
}

/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script 
 *	@param[in] argc argc is a count of the arguments supplied to the command function.
 *	@param[in] argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_Flooding::command(int argc, const char*const* argv)
{
	Tcl& tcl = Tcl::instance();
	
	return Sunset_Routing::command(argc, argv);
}

/*!
 * 	@brief The recv() function is called when a packet is received from the upper/lower layer.
 *	@param p The received packet.
 */

void Sunset_Flooding::recv(Packet *p)
{	
	hdr_ip *iph = HDR_IP(p);
	hdr_cmn *cmh = HDR_CMN(p);
	Packet *tmp;
	
	int src = 0;
	int dst = 0;
	
	src = (int)iph->saddr();
	dst = (int)iph->daddr();
	
	
	/* Packet coming from the upper layer.*/
	
	if (cmh->direction() == hdr_cmn::DOWN) {
		
		forwardPacket(p);
		
		return;
		
	}
	
	Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Flooding::recv receiving something id %d size %d from %d to %d", cmh->uid(), cmh->size(), src, dst);
	
	Sunset_Trace::print_info("rtg - (%f) Node:%d - FLOODING recv something id %d size %d from %d to %d hops %d\n", NOW, getModuleAddress(), cmh->uid(), cmh->size(), src, dst, cmh->num_forwards());
	
	// packet addressed to this node, send it to the upper layer and return
	
	if (dst == getModuleAddress()  && src != getModuleAddress()) {
		
		Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Flooding::recv recv pkt for me");
		
		sendUp(p);
		
		return;
	}
	
	
	// broadcast packet, send it to the upper layer and forrward it
	
	if (dst == Sunset_Address::getBroadcastAddress() && src != getModuleAddress()) {
		
		Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Flooding::recv pkt pkt copied DOPO");
		
		tmp = p->copy();
		Sunset_Utilities::copy_data(p, tmp, getModuleAddress());
		
		
		sendUp(tmp);	  
	}
	
	Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Flooding::recv forward");
	
	forwardPacket(p);
	
	return;
}

/*!
 * 	@brief The forwardPacket() function is called when a packet has to be forwarded.
 *	@param p The packet to forward.
 */

void Sunset_Flooding::forwardPacket(Packet * p)
{
	hdr_cmn *cmh; 
	hdr_ip *iph; 
	
	int src = 0;
	int dst = 0;
	int id = 0;
	
	double rand = 0;
	
	cmh = HDR_CMN(p);
	iph = HDR_IP(p);
	src = (int)iph->saddr();
	dst = (int)iph->daddr();
	id = cmh->uid();
	
	cmh->direction() = hdr_cmn::DOWN;
	
	/* checking if I've already forwarded the same data packet. */
	
	//TODO check multiple commands for remote control that havae to be forwarded
	
	if (pktForwardedInfo.find(src) != pktForwardedInfo.end() && (pktForwardedInfo[src]).find(id) != (pktForwardedInfo[src]).end()) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Flooding::forwardPacket pkt from %d to %d already processed DISCARD", src, dst);
		
		Sunset_Trace::print_info("rtg - (%f) Node:%d - FLOODING duplicated pkt not forwarded: id %d size %d from %d to %d hops %d\n", NOW, getModuleAddress(), cmh->uid(), cmh->size(), src, dst, cmh->num_forwards());
		
		Sunset_Utilities::erasePkt(p, getModuleAddress());
		
		return;
	}
	
	
	// insert this packet in the processed packets structure
	
	Sunset_Trace::print_info("rtg - (%f) Node:%d - FLOODING forwards pkt: id %d size %d from %d to %d hops %d\n", NOW, getModuleAddress(), cmh->uid(), cmh->size(), src, dst, cmh->num_forwards());
	
   	pktForwardedInfo[src].insert(id);
	
  	// I am the packet originator, initialize the data
	if (src == getModuleAddress()) {
		
		cmh->num_forwards() = 0;
		cmh->next_hop_ = Sunset_Address::getBroadcastAddress();
		sendDown(p);
		
		return;
	}
	
	/* checking if the maximum TTL is reached. */
	
	if (cmh->num_forwards() > max_forwards_) {
		
		Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Flooding::forwardPacket pkt for %d TOO MANY FORWARDS DISCARD", dst);
		Sunset_Utilities::erasePkt(p, getModuleAddress());
		
		return;
	}
	
     	rand = Random::uniform();
     	
     	/* computing the probability to forward the data packet. */
     	
	if (rand > probability_) {
		
		Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Flooding::forwardPacket pkt for %d probability DISCARDING %f", dst, rand);
		Sunset_Utilities::erasePkt(p, getModuleAddress());
		
		return;
	}
	
	cmh->next_hop_ = Sunset_Address::getBroadcastAddress();
	
    	Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Flooding::forwardPacket forwarding to node: %d", cmh->next_hop_);
    	
    	/* if the max_random_time_ is set (!= 0) I'll wait a random time before forwarding the data packet. */
	
	if (max_random_time_ > 0.0) {
		
		pktList.push_back(p);
		
		if (!(floodingTimer_->busy())) {
			
			floodingTimer_->start(Random::uniform(max_random_time_));	 
		}
	}
	else {
		
	  	sendDown(p);
	}
	
	return;
}

/*!
 * 	@brief The sendPkt() function sends a packet to the lower layer and remove it from the local data structures.
 */

void Sunset_Flooding::sendPkt() 
{
	Packet *p;
	
	if (pktList.empty()) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Flooding::sendPkt NO PACKET ERROR");
		return;
	}
	
	p = pktList.front();
	pktList.pop_front();
	
	sendDown(p);
	
	if (!(pktList.empty())) {
		
		floodingTimer_->start(Random::uniform(max_random_time_));	 
	}
}

/*!
 * 	@brief The start() function can be called from the TCL scripts to execute routing module operations when the simulation/emulation starts.
 */
void Sunset_Flooding::start()
{
	return;
}

/*!
 * 	@brief The stop() function can be called from the TCL scripts to execute routing module operations when the simulation/emulation stops.
 */
void Sunset_Flooding::stop()
{
	return;
}

