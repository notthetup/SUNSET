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



#ifndef __Sunset_Queue_h__
#define __Sunset_Queue_h__

#include <queue.h>
#include <sunset_utilities.h>
#include <sunset_module.h>
#include <sunset_statistics.h>

class Sunset_Queue;

/*! \brief This class implements a packet queue. */

class Sunset_Queue : public TclObject, public Sunset_Module {
	
public:
	Sunset_Queue();
	
	int command(int argc, const char*const* argv);
	
	virtual void enque(Packet*);    	/*!< \brief Enque packet at the end of the queue */
	virtual void enqueFront(Packet*);   	/*!< \brief Enque packet to the front of the queue */
	virtual Packet* deque();            	/*!< \brief Extract the packet from the front of the queue */
	virtual void reset();
	
	int limit() { return qlim_; }   		/*!< \brief It returns the maximum allowed number of packets in the queue */
	int length() { return pq_->length(); } 		/*!< \brief The number of packets currently in underlying packet queue */
	int byteLength() { return pq_->byteLength(); }	/*!< \brief The number of bytes currently in packet queue */	
	
	Packet* getHead() { return pq_->head(); } /*!< \brief Return the head of the queue */	
	
	virtual void remove(Packet*);   /*!< \brief Remove packet p from  the queue */	
	
	virtual void start();	
	virtual void stop();	
	
private:
	int drop_front_;	/*!< \brief  Drop-from-front (rather than from tail) */
	int qib_;       	/*!< \brief  Bool: if 1 qlimBytes constraint have to be respected */
	int qlimBytes;		/*!< \brief  The maximum allowed size of the queue in bytes (the sum of packet sizes inside the queue does not have to exceed qlimBytes value) */
	int qlim_;		/*!< \brief  The maximum allowed number of packets in the queue */
	int blocked_;		/*!< \brief  Is it blocked now? */
	
	PacketQueue *pq_;	/*!< \brief  A pointer to actual packet queue (maintained by the individual disciplines like DropTail and RED). */
	
	Sunset_Statistics* stat; /*!< \brief  A pointer to the statistics module. */
        
};

#endif 


