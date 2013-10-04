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



#ifndef __Sunset_Mac_h__
#define __Sunset_Mac_h__

#include <sunset_queue.h>
#include <sunset_utilities.h>

#include <sunset_timing.h>
#include <sunset_mac_pkt.h>
#include <sunset_debug.h>
#include <sunset_address.h>
#include <sunset_statistics.h>
#include <sunset_phy2mac-clmsg.h>
#include <sunset_mac2rtg-clmsg.h>
#include <sunset_mac_timers.h>
#include <sunset_module.h>
#include <mmac.h>
#include <sunset_information_dispatcher.h>

#define MAX(X,Y) ( X > Y ? X : Y ) 
#define MIN(X,Y) ( X > Y ? Y : X ) 
#define EPSILON_DELAY 0.1

#define RX_TIMEOUT  8

typedef enum mac_action_type {  //Possible actions at the MAC layer 
	SUNSET_MAC_TX_ACTION_OK             = 1,
	SUNSET_MAC_TX_ACTION_DONE           = 2,
	SUNSET_MAC_TX_ACTION_ABORTED        = 3,
	SUNSET_MAC_TX_ACTION_BUSY           = 4,
	SUNSET_MAC_TX_ACTION_ERROR          = 5,
	SUNSET_MAC_RX_ACTION_OK             = 6,
	SUNSET_MAC_RX_ACTION_DONE	    = 7,
	SUNSET_MAC_RX_ACTION_ERROR          = 8,
	SUNSET_MAC_ACTION_PKT_DISCARDED     = 9
} mac_action_type;

class Sunset_Timing;
class Sunset_Queue;

/*! @brief The generic MAC layer class. It is not a complete MAC to be used but it implements basic funstionalities needed by the specific MAC protocol. */

class Sunset_Mac : public MMac, public Sunset_Module {
	
	friend class Sunset_Mac_Timer;	
	
public: 
	Sunset_Mac();
	
	/*! @brief Return the module_address value. */
	inline int addr() { return getModuleAddress(); }
	
	/*! @brief Return the broadcast address defined in the  Sunset_Address class. */
	virtual int getBroadcastAddress() { return Sunset_Address::getBroadcastAddress(); }
	
	virtual int recvSyncClMsg(ClMessage* m);
	
	// functions used for the interaction with the routing layer
	//--------------------------------------
	void Mac2RtgPktTransmitted(const Packet* p);
	void Mac2RtgPktDiscarded(const Packet* p);
	//--------------------------------------
	
protected:
	
	virtual int command(int argc, const char*const* argv);
	
	virtual void recvFromUpperLayers(Packet* p);
	
	virtual void start();	
	
	virtual void stop();	
	
	// functions used for the interaction with the lower layer
	//--------------------------------------
	virtual void Phy2MacEndTx(const Packet* p);
	virtual void Phy2MacTxAborted(const Packet* p);
	
	virtual void Phy2MacStartRx(const Packet* p);
	virtual void Phy2MacEndRx(Packet* p);
	//--------------------------------------
	
	/*! @brief Function called when the node starts the operation to transmit packet p. */
	virtual void send(Packet* p);
	
	/*! @brief Function called to initialize the packet p with MAC header information */
	virtual void createData(Packet* p);
	
	/*! @brief Function called if there are packets waiting to be transmitted. */
	virtual void anotherTransmission();
	
	/*! @brief The getMacHdrSize returns the MAC packet header size. */
	virtual int getMacHdrSize() { return MAC_HDR_SIZE; }
	
	/*! @brief The setMacHdrSize sets the MAC packet header size. */
	virtual void setMacHdrSize(int size) { MAC_HDR_SIZE = size; }
	
	// reference to the queue module
	Sunset_Queue* macQueue_;
	
	MacState	rx_state_;	// incoming state (MAC_RECV or MAC_IDLE)
	
	MacState	tx_state_;	// outgoint state
	
	int tx_active_;     // 1 if a tx is on going, 0 otherwise
	
	int tx_retry;       // number of retry for the current packet
	
	Packet* pktTx_;
	Packet* pktRx_;
	
	// reference to timing module    
	Sunset_Timing* macTiming; 
	
	// mac header size used in simulation
	int MAC_HDR_SIZE;
	
	// reference to the information dispatcher module
	Sunset_Information_Dispatcher* sid;
	
	//ID assigned by the information dispatcher
	int sid_id;
	
	// reference to the statsitic module
	Sunset_Statistics* stat;
	
	//Used by the information dispatcher to notify to the mac if a value is changed among the ones the mac has registered itsef for
	virtual int notify_info(list<notified_info> linfo);
	
	/*! @brief Function called to control if the underwater channel is idle or tx/rx operations are ongoing. */
	virtual int is_idle(void);
	
	/*! @brief Function called to discard packet p */
	virtual void discard(Packet *p);
	
	/*! @brief Function called to set MAC rx state*/
	virtual void setRxState(MacState newState);
	
	/*! @brief Function called to set MAC tx state*/
	virtual void setTxState(MacState newState);
	
	/*! @brief Function called to check if there packet in queue waiting to be transmitted*/
	virtual void checkQueue(Packet* p) ;	
	
	/*! @brief Function called when a transmission operation is perfomed to check if an action has to be taken*/
	virtual void txAction(Packet* p, mac_action_type mct);
	
	/*! @brief Function called when a reception operation is perfomed to check if an action has to be taken*/
	virtual void rxAction(Packet* p, mac_action_type mct);
	
	
};

#endif
