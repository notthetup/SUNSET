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


#ifndef __Sunset_Agent_h__
#define __Sunset_Agent_h__

#include <math.h>
#include <assert.h>
#include <module.h>
#include <node-core.h>
#include <sunset_agent_pkt.h>
#include <sunset_utilities.h>
#include <sunset_module.h>
#include <sunset_address.h>
#include <sunset_statistics.h>
#include <sunset_debug.h>
#include <sunset_trace.h>
#include <sunset_common_pkt.h>

#define AGT_MAX_BUFFER_SIZE 3000

typedef enum agt_action_type {  //Possible actions at the APPLICATION layer 
	SUNSET_AGT_TX_ACTION			= 1,
	SUNSET_AGT_RX_ACTION			= 2,
	SUNSET_AGT_RX_DUPLICATED_ACTION		= 3
} agt_action_type;

/*! \brief The generic Agent layer class. It supports also the creation of data packets. */

class Sunset_Agent: public Module, public Sunset_Module 
{ 
	
public:
	Sunset_Agent();
	~Sunset_Agent();
	
	virtual int command( int argc, const char*const* argv );
	
	virtual void recv(Packet *p);
	
	virtual bool isPktForMe(Packet* p);
	
	virtual int getPortNumber() { return port_number; }
	
protected:
	
	// These functions compute basic statistic information to be able to provide a feedback to the user even if the statistic module is not used
	//------------------------------------
	int getTxPkt(int dest) ;
	
	int getPktHop(int src);
	
	int getRxPkt(int src);
	
	double getDelay(int src);
	//------------------------------------
	
	virtual void start();
	
	virtual void stop(); 
	
	/*! @brief It is used when receiving a packet from the lower layer */
	virtual void recvPkt(Packet *p);
	
	/*! @brief It is used when receiving a data packet from the lower layer */
	virtual void recvData( Packet * p );
	
	/*! @brief Create a packet initializing all the packet header information needed when running in simulation mode. */
	Packet* createPacket(int dest, int len, int pktType, int pktSubType);
	
	/*! @brief Create a packet initializing all the packet header information needed when running in emulation mode, including the packet payload (msg)*/
	Packet* createPacket(int dest, char* msg, int len, int pktType, int pktSubType);
	
	/*! @brief Initialize all the packet header information needed when running in simulation mode. */
	void initPacket(Packet* p, int dest, int size, int pktType, int pktSubType);
	
	/*! @brief Send a packet to dest when running in simulation mode. */
	virtual void sendDataSim(int dest, int len);
	
	/*! @brief Send a packet with preconfigured payload to dest when running in emulation mode. */
	virtual void sendDataEmulation(int dest);
	
	/*! @brief Send a packet with the given payload (msg) to dest when running in emulation mode. */
	virtual void sendData(int dest, char* msg);
	
	/*! @brief Create and send a packet with random size (fixed_size + rand<min_size, max_size>) to dest. When running in emulation mode a random pyalod is created. */
	virtual int sendRandomLength(int destId, int fix_size, int min_size, int max_size);
	
	/*! @brief Create, initialize and transmit the packet towards the netowrk stack and initialize statistic variables. */
	virtual void sendPkt(int dest, char* msg, int sizeMsg, int type, int subType);
	
	//port number addigned to the agent module in the tcl script if multiplexing and demultiplexing operations are performed (in case multiple agent are created at the application layer)
	int port_number;	
	
	// Size of the agent header when running in simulaiton mode
	int getAgtHeaderSize();
	
	// packet id used to identify the different packet transmitted by the agent module
	static int pkt_num;
	
	// maximal packet size in bytes, defined using the tcl scripts
	int MAX_DATA_SIZE;
	
	// reference to the information dispatcher module
	Sunset_Information_Dispatcher* sid;
	
	//ID assigned by the information dispatcher
	int sid_id;
	
	//Used by the information dispatcher to notify to the agent if a value is changed among the ones the agent has registered itsef for
	int notify_info(list<notified_info> linfo);
	

	/*! @brief Function called when a transmission operation is perfomed to check if an action has to be taken*/
	virtual void txAction(Packet* p, agt_action_type mct);
	
	/*! @brief Function called when a reception operation is perfomed to check if an action has to be taken*/
	virtual void rxAction(Packet* p, agt_action_type mct);

	// reference to the statsitic module
	Sunset_Statistics* stat;
	
	
	int pkt_recv_;
	int pkt_gen_;
	map<int, int> pktSent_;
	map<int, int> pktHop_;
	map<int, int> pktRx_;
	map<int, double> pktLatency_;
	map<int, set<int> > pktReceived;
	
	int start_pktId;
};

#endif
