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


#ifndef __Sunset_Statistics_h__
#define __Sunset_Statistics_h__

#include <map>
#include <set>
#include <list>
#include <stdlib.h>
#include <mac.h>
#include <packet.h>
#include <sunset_debug.h>
#include <iostream>
#include <fstream>
#include <stdio.h>

#define TIME2INT 	10000

/*!
 @brief This data structure defines the possible actions we want to capture in our statistic analysis.
 */
typedef enum  {
	
	SUNSET_DEBUG = -1,
	SUNSET_STAT_AGENT_TX = 1,
	SUNSET_STAT_AGENT_RX = 2,
	SUNSET_STAT_ENQUE = 3,
	SUNSET_STAT_QUEUE_DISCARD = 4,
	SUNSET_STAT_DEQUE = 5, 
	SUNSET_STAT_MAC_BACKOFF = 6,
	SUNSET_STAT_MAC_TX = 7,
	SUNSET_STAT_MAC_DISCARD = 8,
	SUNSET_STAT_MAC_RX = 9,
	SUNSET_STAT_MODEM_GET_PKT = 10,
	SUNSET_STAT_MODEM_TX_START = 11,
	SUNSET_STAT_MODEM_TX_DONE = 12,
	SUNSET_STAT_MODEM_RX_START = 13,
	SUNSET_STAT_MODEM_RX_DONE = 14,
	SUNSET_STAT_MODEM_RX_PKT = 15,
	SUNSET_STAT_MODEM_INFO = 16,
	SUNSET_STAT_MAC_NAV = 17,
	SUNSET_STAT_MODEM_TX_ABORTED	= 18,
	SUNSET_STAT_MODEM_TRANSFERT_START = 19,
	SUNSET_STAT_MODEM_TRANSFERT_END = 20,
	SUNSET_STAT_MODEM_RX_ABORTED	= 21,
	SUNSET_STAT_MAC_CHANNEL_BUSY = 22,
	SUNSET_STAT_MAC_NO_REPLY = 23,
	SUNSET_STAT_MAC_RX_ERROR = 24,
	SUNSET_STAT_MAC_TX_DONE = 25,
	SUNSET_STAT_MAC_TX_ABORTED = 26,
	SUNSET_STAT_MAC_NEW = 27,
	SUNSET_STAT_ENERGY_TX = 28,
	SUNSET_STAT_ENERGY_RX = 29,
	SUNSET_STAT_ENERGY_IDLE = 30
	
} sunset_statisticType;


/*! @brief The generic statistics class is used by the other modules to trace what happens during the simulation/emulation. The information are written on a trace file for post-processing analysis. The Sunset_Protocol_Statistics extends this class to trace all the network protocols statistics. 
 * @see class Sunset_Protocol_Statistics
 */

class Sunset_Statistics: public TclObject 
{
	
public:
	
	static Sunset_Statistics *single;
	
	Sunset_Statistics();
	~Sunset_Statistics() { single = NULL; } 
	
	static Sunset_Statistics* instance();
	
	virtual int command( int argc, const char*const* argv );
	
	/*!
	 * 	@brief The logStatInfo function stores the information related to a specified packet for a specfied action by a node at a given time.
	 */
	virtual void logStatInfo(sunset_statisticType sType, u_int16_t node, Packet* p, double time, const char *fmt, ...);
	
	static bool use_stat() {
		
		return useStat;
	}
	
	virtual void start();
	virtual void stop();
	
protected:
	
	static int useStat;
};

#endif
