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

#include <sunset_aloha.h>
#include <sunset_trace.h>

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_AlohaClass : public TclClass 
{
public:
	
	Sunset_AlohaClass() : TclClass("Module/MMac/Sunset_Aloha") {}
	
	TclObject* create(int, const char*const*) {
		
		return (new Sunset_Aloha());
	}
	
} class_Sunset_Aloha;


Sunset_Aloha::Sunset_Aloha() : Sunset_Mac()
{
}

/*!
 * 	@brief The start() function can be called from the TCL script to execute MAC module operations when the simulation/emulation starts.
 */

void Sunset_Aloha::start()
{
	Sunset_Mac::start();
	
	// Register the aloha module to the information dispatcher and collect the assigned ID
	
	if (sid != NULL) {
		
		sid_id = sid->register_module(getModuleAddress(), "MAC_ALOHA", this);
		
		sid->define(getModuleAddress(),sid_id, "MAC_TX_COMPLETED");
		sid->define(getModuleAddress(),sid_id, "MAC_TX_ABORT");
		
		sid->provide(getModuleAddress(),sid_id, "MAC_TX_COMPLETED");
		sid->provide(getModuleAddress(),sid_id, "MAC_TX_ABORT");

	}
	
	return;
}

/*!
 * 	@brief The stop() function can be called from the TCL script to execute MAC module operations when the simulation/emulation stops.
 */

void Sunset_Aloha::stop()
{
	Sunset_Mac::stop();
	
	return;
}

/*!
 * 	@brief The anotherTransmission function checks if packet queue is empty or not. If the queue is not empty a data packet is transmitted.
 */

void Sunset_Aloha::anotherTransmission() 
{
	
	if (macQueue_ != 0 && macQueue_->length() > 0) {
		
		Packet* p2;
		
		Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Aloha::anotherTransmission queue length %d", macQueue_->length());
		
		p2 = macQueue_->deque();
		
		pktTx_ = p2;
		
		Mac2PhyStartTx(p2->copy());
		
		tx_active_ = 1;
Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Aloha::Mac2PhyStartTx1 p2 %p pktTx_ %p", p2, pktTx_);
		
	} else {
		
		Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Aloha::anotherTransmission no pkt in queue");
	}


Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Aloha::Mac2PhyStartTx pktTx_ %p", pktTx_);

}

/*!
 * 	@brief The send function sends a packet to the lower layer.
 *	@param p The packet to be sent.
 */

void Sunset_Aloha::send(Packet* p) 
{
	createData(p);
	
	if (tx_active_) {
		
		if (macQueue_ != 0) {
			
			macQueue_->enque(p);
			
			Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Aloha::send enque packet, queue length %d", macQueue_->length());
		}
		else {
			
			Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Aloha::send no queue discard pkt");
			
			Mac2RtgPktDiscarded(p->copy());
			
			Sunset_Utilities::erasePkt(p, getModuleAddress());
		}
	}
	else {
		pktTx_ = p;
		
		txAction(p, SUNSET_MAC_TX_ACTION_OK);
		
		Mac2PhyStartTx(p->copy()); 
		
		tx_active_ = 1;
		
		Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Aloha::send transmit pkt to %d", HDR_SUNSET_MAC(p)->dst);
	}
}

/*! @brief Function called when a transmission operation is performed to check if an action has to be taken.
 *	@param[in] p   The packet the action is related to.
 *	@param[in] mct The transmission action that has been executed.
 */

void Sunset_Aloha::txAction(Packet* p, mac_action_type mct) 
{
	
	if (p == 0) {
		
		return;
	}

	hdr_cmn *ch = HDR_CMN(p);
	hdr_ip *ip = HDR_IP(p);
	struct hdr_Sunset_Mac *mh = HDR_SUNSET_MAC(p);
	int dst = (mh->dst);
	int src = mh->src; 
	u_int8_t  type = mh->dh_fc.fc_type;
	u_int8_t  subtype = mh->dh_fc.fc_subtype;
	string pktType = "";
	string actionType = "";
	
	Sunset_Mac::txAction(p, mct);
	
	switch(type) {
		
		case SUNSET_MAC_Type_Control:
		
			switch(subtype) {
			
				default:
					
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Aloha::txAction Invalid MAC Control Subtype %x ERROR", subtype);
					
					return;
			}
			
			break;
			
		case SUNSET_MAC_Type_Data:
			
			switch(subtype) {
			
				case SUNSET_MAC_Subtype_Data:
				
					pktType = "DATA";
					break;
					
				default:
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Aloha::txAction Invalid MAC Data Subtype %x ERROR", subtype);
					
					return;
			}
			
			break;
			
		default:
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Aloha::txAction Invalid MAC Type %x ERROR", subtype);
			
			return;
	}
	
	switch (mct) {
		
		case SUNSET_MAC_TX_ACTION_OK:
			
			actionType = "TX";
			
			break;
			
		case SUNSET_MAC_TX_ACTION_DONE:
                        
			actionType = "TX COMPLETED";
			
			break;
			
		case SUNSET_MAC_TX_ACTION_ABORTED:
			
			actionType = "ABORTED";
			
			break;
			
		case SUNSET_MAC_TX_ACTION_BUSY:
			
			actionType = "ERROR";
			
			break;
			
		case SUNSET_MAC_TX_ACTION_ERROR:
			
			actionType = "ERROR";
			
			break;
			
		case SUNSET_MAC_ACTION_PKT_DISCARDED:
			
			actionType = "DISCARDED";
			
			break;
			
		default:
			
			actionType = "ERROR";
			
			break;
	}
	
	Sunset_Trace::print_info("mac - (%f) Node:%d - ALOHA %s %s source %d destination %d src %d dst %d\n", NOW, getModuleAddress(), actionType.c_str(), pktType.c_str(), ip->saddr(), ip->daddr(), src, dst);
	
	return;
}

/*! @brief Function called when a reception operation is performed to check if an action has to be taken.
 *	@param[in] p   The packet the action is related to.
 *	@param[in] mct The reception action that has been executed.
 */

void Sunset_Aloha::rxAction(Packet* p, mac_action_type mct) 
{
	Sunset_Mac::rxAction(p, mct);

	if (p == 0) {
		
		return;	
	}
	
	hdr_cmn *ch = HDR_CMN(p);
	hdr_ip *ip = HDR_IP(p);
	struct hdr_Sunset_Mac *mh = HDR_SUNSET_MAC(p);
	int dst = (mh->dst);
	int src = mh->src; 
	u_int8_t  type = mh->dh_fc.fc_type;
	u_int8_t  subtype = mh->dh_fc.fc_subtype;
	string pktType = "";
	string actionType = "";
		
	switch(type) {
		case SUNSET_MAC_Type_Control:
			
			switch(subtype) {
			
				default:
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Aloha::rxAction Invalid MAC Control Subtype %x ERROR", subtype);
					
					return;
			}
			
			break;
			
		case SUNSET_MAC_Type_Data:
			
			switch(subtype) {
			
				case SUNSET_MAC_Subtype_Data:
					pktType = "DATA";
					break;
				
				default:
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Aloha::rxAction Invalid MAC Data Subtype %x ERROR", subtype);
					
					return;
			}
			
			break;
			
		default:
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Aloha::rxAction Invalid MAC Type %x ERROR", subtype);
			
			return;
	}
	
	switch (mct) {
		
		case SUNSET_MAC_RX_ACTION_OK:
			
			actionType = "RX";
			
			break;
			
		case SUNSET_MAC_RX_ACTION_DONE:
			
			actionType = "RX_DONE";
			
			break;
			
		case SUNSET_MAC_RX_ACTION_ERROR:
			
			actionType = "RX_ERROR";
			
			break;
			
		default:
			
			actionType = "RX_ERROR";
			
			break;
	}
	
	
	Sunset_Trace::print_info("mac - (%f) Node:%d - ALOHA %s %s source %d destination %d src %d dst %d\n", NOW, getModuleAddress(), actionType.c_str(), pktType.c_str(), ip->saddr(), ip->daddr(), src, dst);
	
}

void Sunset_Aloha::Phy2MacEndTx(const Packet* p) {
	
	Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Aloha::Phy2MacEndTx %p %p", p, pktTx_);
	
	if (sid == NULL) {
		
		Sunset_Mac::Phy2MacEndTx(p);
		
		return;
	}
	
	notified_info ni;
	ni.node_id = getModuleAddress();
	ni.info_time = NOW;
	ni.info_name = "MAC_TX_COMPLETED";
	ni.info_size = 0;
	
	if ( sid->set(getModuleAddress(), sid_id, ni) == 0 ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Aloha::sendMacTxCompleted PROVIDING INFO %s NOT DEFINED", (ni.info_name).c_str());
		Sunset_Mac::Phy2MacEndTx(p);
		return;
	}
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Aloha::sendMacTxCompleted node %d time %f", 
				getModuleAddress(), NOW);
	
	Sunset_Mac::Phy2MacEndTx(p);

}


void Sunset_Aloha::Phy2MacTxAborted(const Packet* p) {
	
	if (sid == NULL) {
		
		Sunset_Mac::Phy2MacTxAborted(p);
		return;
	}
	
	notified_info ni;
	ni.node_id = getModuleAddress();
	ni.info_time = NOW;
	ni.info_name = "MAC_TX_ABORT";
	ni.info_size = 0;
	
	if ( sid->set(getModuleAddress(), sid_id, ni) == 0 ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Aloha::sendMacTxAbort PROVIDING INFO %s NOT DEFINED", (ni.info_name).c_str());
		Sunset_Mac::Phy2MacTxAborted(p);
		return;
	}
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Aloha::sendMacTxAbort node %d time %f", 
				getModuleAddress(), NOW);
	
	Sunset_Mac::Phy2MacTxAborted(p);
}