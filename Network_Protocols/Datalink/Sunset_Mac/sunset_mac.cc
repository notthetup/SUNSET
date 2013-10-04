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



#include <sunset_mac.h>
#include<iostream>
#include <sunset_trace.h>

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_MacClass : public TclClass 
{
public:
	
	Sunset_MacClass() : TclClass("Module/MMac/Sunset_Mac") {}
	
	TclObject* create(int, const char*const*) {
		
		return (new Sunset_Mac);
	}
	
} class_Sunset_Mac;


Sunset_Mac::Sunset_Mac() : macQueue_(0), macTiming(0)
{
	MAC_HDR_SIZE = 0;
	
	// Get variables initialization from the Tcl script
	
	bind("moduleAddress", &module_address);
	
	bind("runId", &run_id);
	
	bind("MAC_HDR_SIZE", &MAC_HDR_SIZE);
	
	sid = NULL;
	
	sid_id = -1;
	
	pktTx_ = 0;
	pktRx_ = 0;
	
	tx_state_ = rx_state_ = MAC_IDLE;
	
	tx_active_ = 0;
	
	tx_retry = 0;
	
	Random::seed_heuristically();
	
	stat = NULL;
}

/*!
 * 	@brief The recvFromUpperLayers function handles a packet coming from upper layers.
 *	@param p The received packet.
 */

void Sunset_Mac::recvFromUpperLayers(Packet* p)
{  
	Sunset_Debug::debugInfo(5, getModuleAddress(),  "Sunset_Mac::recvFromUpperLayers");
	
	send(p);
}

/*!
 * 	@brief The Phy2MacTxAborted function handles the abort-of-PHY-transmission event.
 *	@param p The aborted packet.
 */

void Sunset_Mac::Phy2MacTxAborted(const Packet* p)
{
	Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Mac::Phy2MacTxAborted");
	
	tx_active_ = 0;
	
	Packet * aux = 0;

	if (pktTx_ != 0) {
		
		aux = pktTx_->copy();

		txAction(pktTx_, SUNSET_MAC_ACTION_PKT_DISCARDED);
		
		Sunset_Utilities::erasePkt(pktTx_, getModuleAddress());
	}
		
	pktTx_ = 0;
	
	if (aux != 0) {

		Mac2RtgPktDiscarded(aux);
		
	}

	Sunset_Utilities::eraseOnlyPkt((Packet*)p, getModuleAddress());

	anotherTransmission();
}

/*!
 * 	@brief The anotherTransmission function is used to check if there are packets waiting to be transmitted. It has to be implemented by  the specific MACs.
 */

void Sunset_Mac::anotherTransmission() 
{
	
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Mac::anotherTransmission");
}

/*!
 * 	@brief The Phy2MacEndTx function handles the end-of-PHY-transmission event.
 *	@param p The transmitted packet.
 */

void Sunset_Mac::Phy2MacEndTx(const Packet* p)
{
	Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Mac::Phy2MacEndTx pktTx_ %p", pktTx_);
	
	tx_active_ = 0;
	
	Packet * aux = 0;
	
	if (pktTx_ != 0) {
		
		txAction(pktTx_, SUNSET_MAC_TX_ACTION_DONE);
		
		aux = pktTx_->copy();
				
		Sunset_Utilities::erasePkt(pktTx_, getModuleAddress());
		
		pktTx_ = 0;
				
	}
		
	if (aux != 0) {

		Mac2RtgPktTransmitted(aux);
		
	}

	Sunset_Utilities::eraseOnlyPkt((Packet*)p, getModuleAddress());

	anotherTransmission();
	
}

/*!
 * 	@brief The Phy2MacStartRx function handles the detected-start-of-PHY-reception event.
 *	@param p The packet under reception.
 */

void Sunset_Mac::Phy2MacStartRx(const Packet* p)
{
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Mac::Phy2MacStartRx");
}

/*!
 *  @brief The Phy2MacEndRx function handles the end-of-PHY-reception event.
 *	@param p The received packet.
 */

void Sunset_Mac::Phy2MacEndRx(Packet* p)
{
	
	if ( p == 0 ) {
		
		setRxState(MAC_IDLE);
		
		return;

	}
	
	hdr_cmn* ch = HDR_CMN(p);
	
	struct hdr_Sunset_Mac *mh = HDR_SUNSET_MAC(p);
	
	u_int8_t dst = (mh->dst);
	u_int8_t src = mh->src; 
	u_int8_t type = mh->dh_fc.fc_type;
	u_int8_t subtype = mh->dh_fc.fc_subtype;
	
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Mac::Phy2MacEndRx src %d dest %d size %d", src, dst, ch->size());
	
	if (ch->error()) {
		
		if (ch->size() == 0) {
			
			Sunset_Utilities::eraseOnlyPkt(p, getModuleAddress());
			setRxState(MAC_IDLE);
			
			return;
		}

		rxAction(p, SUNSET_MAC_RX_ACTION_ERROR);

		if (dst == getModuleAddress() || dst == Sunset_Address::getBroadcastAddress()) {
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Mac::Phy2MacEndRx pkt for me Error src %d dest %d", src, dst);
		}
		else {
			
			Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Mac::Phy2MacEndRx pkt Error src %d dest %d", src, dst);        
		}
		
		Sunset_Utilities::erasePkt(p, getModuleAddress());
		
		return;
	}
	
	if (p != 0) {
		
		rxAction(p, SUNSET_MAC_RX_ACTION_OK);
	}
	
	
	if (dst == getModuleAddress() || dst == getBroadcastAddress()) {
		
		ch->size() -= getMacHdrSize();
		ch->num_forwards() += 1;
		
		rxAction(p, SUNSET_MAC_RX_ACTION_DONE);
		
		sendUp(p);
	}
	else {
		
		Sunset_Utilities::erasePkt(p, getModuleAddress());
		
		return;	
	}
}

/*!
 *  @brief The RtgPktTransmitted function must be called by the MAC to inform the Routing that a packet has been transmitted.
 *	@param p The transmitted packet.
 */

void Sunset_Mac::Mac2RtgPktTransmitted(const Packet* p)
{
	ClMsgMac2RtgPktTransmitted m(p);
	sendSyncClMsgUp(&m);
}

/*!
 *  @brief The RtgPktTransmitted function must be called by the MAC to inform the Routing that a packet has been discarded.
 *	@param p The discarded packet.
 */

void Sunset_Mac::Mac2RtgPktDiscarded(const Packet* p)
{
	ClMsgMac2RtgPktDiscarded m(p);
	sendSyncClMsgUp(&m);
}

/*!
 * 	@brief The start() function can be called from the TCL scripts to execute MAC module operations when the simulation/emulation starts.
 */

void Sunset_Mac::start()
{
	Sunset_Module::start();
	
	// Initialize the information dispatcher module reference
	sid = Sunset_Information_Dispatcher::instance();    
	
	// if statistics info are used initialize the statistics module reference
	if (Sunset_Statistics::use_stat()) {
		
		stat = Sunset_Statistics::instance();   
	}
	
	return;
}

/*!
 * 	@brief The stop() function can be called from the TCL scripts to execute MAC module operations when the simulation/emulation stops.
 */

void Sunset_Mac::stop()
{
	Sunset_Module::stop();
	
	//information dispatcher initialization
	sid = NULL;
	
	sid_id = -1;
	
	//statistics module initialization
	stat = NULL;
	
	return;
}

/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script 
 *	@param[in] argc argc is a count of the arguments supplied to the command function.
 *	@param[in] argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_Mac::command(int argc, const char*const* argv)
{
	Tcl& tcl = Tcl::instance();
	
	if (argc == 2) {
		
		/* The "start" command starts the MAC module */
		
		if (strcmp(argv[1], "start") == 0) {
			
			Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_Agent::command start Module");
			
			start();
			
			return TCL_OK;
		}
		
		/* The "stop" command stops the MAC module */
		
		if (strcmp(argv[1], "stop") == 0) {
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Agent::command stop Module");
			
			stop();
			
			return TCL_OK;
		}
	}
	else if ( argc == 3 ) {
		
		/* The "setModuleAddress" command sets the address of the MAC module. */		
		
		if (strcmp(argv[1], "setModuleAddress") == 0) {
			
			module_address = atoi(argv[2]);
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Agent::command setModuleAddress %d", getModuleAddress());
			return TCL_OK;
		}
		
		/* The "setQueue" command sets the queue of the MAC module. */
		
		if (strcmp(argv[1], "setQueue") == 0) {
			
			macQueue_ = (Sunset_Queue*) TclObject::lookup(argv[2]);
			
			Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Mac::command setQueue");
			
			assert(macQueue_);
			
			return (TCL_OK);
		}
		
		/* The "setTiming" command sets the timing class of the MAC module. */
		
		if (strcmp(argv[1], "setTiming") == 0) {
			
			macTiming = (Sunset_Timing*) TclObject::lookup(argv[2]);
			
			Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Mac::command setTiming");
			
			assert(macTiming);
			
			return (TCL_OK);
		}
	}	
	else if ( argc == 4 ) {
		
		/* The "setModuleAddress" command sets the address of the agent module. */
		
		if (strcmp(argv[1], "setModuleAddress") == 0) {
			
			module_address = atoi(argv[2]);
			run_id = atoi(argv[3]);
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Mac::command setModuleAddress %d runId %d", getModuleAddress(), getRunId());
			return TCL_OK;
		}
	}
	
	return MMac::command(argc, argv);
}

/*!
 * 	@brief The send function sends a packet to the lower layer. It has to be implemented by the the specific MAC.
 *	@param p The packet to be sent.
 */

void Sunset_Mac::send(Packet* p) 
{
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Mac::send");
}

/*!
 * 	@brief The createData function adds the MAC header information to the packet to be sent.
 *	@param p The packet to be sent.
 */

void Sunset_Mac::createData(Packet* p) 
{
	struct hdr_Sunset_Mac* dh = HDR_SUNSET_MAC(p);
	dh->src = getModuleAddress();
	dh->dst = HDR_CMN(p)->next_hop();
	
	hdr_cmn* ch = HDR_CMN(p);
	int dst = (dh->dst);
	
	/*
	 * Update the MAC header
	 */
	
	ch->size() += getMacHdrSize();
	
	dh->dh_fc.fc_protocol_version = SUNSET_MAC_ProtocolVersion;
	dh->dh_fc.fc_type       = SUNSET_MAC_Type_Data;
	dh->dh_fc.fc_subtype    = SUNSET_MAC_Subtype_Data;
	
	dh->dh_fc.fc_to_ds	= 0;
	dh->dh_fc.fc_from_ds	= 0;
	
	dh->dh_fc.fc_more_frag  = 0;
	dh->dh_fc.fc_retry      = 0;
	dh->dh_fc.fc_pwr_mgt    = 0;
	dh->dh_fc.fc_more_data  = 0;
	dh->dh_fc.fc_wep        = 0;
	dh->dh_fc.fc_order      = 0;
	
	//dh->duration = 0;
	
	/* store data tx time */
 	ch->txtime() = macTiming->txtime(macTiming->getPktSize(p), TIMING_DATA_RATE);
	
	
	Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Mac::createData Src %d Dest %d Type %d SubType %d pkt_type %d", dh->src, dh->dst, dh->dh_fc.fc_type, dh->dh_fc.fc_subtype, ch->ptype());
	
	return;
}

/*!
 * 	@brief The Phy2recvSyncClMsg function handles the reception of synchronous messages coming from the PHY layer.
 *	@param m The synchronous messages coming from the PHY layer.
 */

int Sunset_Mac::recvSyncClMsg(ClMessage* m)
{
	
	/* If the data packet transmission has been aborted ... */
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Mac::recvSyncClMsg mType %d", m->type());
	
	if (m->type() == CLMSG_PHY2MAC_TXABORTED) {
		
		Phy2MacTxAborted(((ClMsgPhy2MacTxAborted*)m)->pkt);
		
		return 0;
	}
	else {
		return MMac::recvSyncClMsg(m);
	}
}

/*!
 * 	@brief The notify_info function is called from the information dispatcher when a value the mac module is registered 
 *  for has been updated. This mac is currently not registered for any variables, this method can be extend by other 
 *  mac protocols if interested in any variables shared with the other modules
 *	@param linfo The list of variables shared with the other modules the mac module is interested in.
 */

int Sunset_Mac::notify_info(list<notified_info> linfo) 
{ 
	return Sunset_Module::notify_info(linfo); 
} 

/*!
 * 	@brief The is_idle() function checks if the channel is idle or not (carrier sensing).
 *	@retval 0 The channel is idle.
 * 	@retval 1 The channel is not busy.
 */

int Sunset_Mac::is_idle() 
{
	int isIdle = 1;
	ClMsgMac2PhyGetIsIdle msg;
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Mac::is_idle() rxState %d txState %d", rx_state_, tx_state_);
	
	if(rx_state_ != MAC_IDLE) {
		
		return 0;
	}
	
	if(tx_state_ != MAC_IDLE || tx_active_ != 0) {
		
		return 0;
	}
	
	sendSyncClMsg(&msg);
	isIdle = msg.getIsIdle();
	
	if(!isIdle) {
		
		Sunset_Debug::debugInfo(0, getModuleAddress(), "Sunset_Mac::is_idle() passes al check but is receiving");
		
		return 0;
	}
	
	return 1;
}

/*!
 * 	@brief The checkQueue() functions enqueues (if a packet queue is defined) or sends directly a data packet received from the upper layers.
 * 	@param[in] p The packet to be enqueued/sent.
 */

void Sunset_Mac::checkQueue(Packet* p) 
{
	
	createData(p);
	
	if (macQueue_ != 0) {
		
		if (pktTx_ != 0) {
			
			macQueue_->enque(p);
			
			Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Mac::checkQueue queueLength %d", macQueue_->length());
			
			return;
		}
		else {
			
			send(p);
		}
	}
	else {
		if (pktTx_ != 0) {
			
			discard(p);
		}
		else {
			
			send(p);
		}
	}
}

/*!
 * 	@brief The discard() function erases a packet.
 *	@param[in] p The packet to be erased.
 */

void Sunset_Mac::discard(Packet *p) 
{
	Sunset_Utilities::erasePkt(p, getModuleAddress());
}

/*!
 * 	@brief The setRxState() function sets the rx_state to a new state.
 *	@param[in] newState The new state.
 */

void Sunset_Mac::setRxState(MacState newState) 
{
	rx_state_ = newState;
}

/*!
 * 	@brief The setTxState() function sets the tx_state to a new state.
 *	@param[in] newState The new state.
 */

void Sunset_Mac::setTxState(MacState newState) 
{
	tx_state_ = newState;
}

/*! @brief Function called when a transmission operation is performed to check if an action has to be taken
 *	@param[in] p   The packet the action is related to.
 *	@param[in] mct The transmission action that has been executed.
 */
void Sunset_Mac::txAction(Packet* p, mac_action_type mct)
{
	
	switch (mct) {
		
		case SUNSET_MAC_TX_ACTION_OK:
			
			if (Sunset_Statistics::use_stat() && stat != NULL) {
				
				stat->logStatInfo(SUNSET_STAT_MAC_TX, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");	
			}
			
			break;
			
		case SUNSET_MAC_TX_ACTION_DONE:
                        
			if (Sunset_Statistics::use_stat() && stat != NULL) {
				
				stat->logStatInfo(SUNSET_STAT_MAC_TX_DONE, getModuleAddress(), p, 
						HDR_CMN(p)->timestamp(), "");	
				
			}
			break;
			
		case SUNSET_MAC_TX_ACTION_ABORTED:
			
			if (Sunset_Statistics::use_stat() && stat != NULL) {
				
				stat->logStatInfo(SUNSET_STAT_MAC_CHANNEL_BUSY, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");
				
				stat->logStatInfo(SUNSET_STAT_MAC_TX_ABORTED, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");	
				
			}
			
			break;
			
		case SUNSET_MAC_TX_ACTION_BUSY:
			
			if (Sunset_Statistics::use_stat() && stat != NULL) {
				
				stat->logStatInfo(SUNSET_STAT_MAC_CHANNEL_BUSY, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");
				
			}
			break;
			
		case SUNSET_MAC_TX_ACTION_ERROR:
			
			if (Sunset_Statistics::use_stat() && stat != NULL) {
				stat->logStatInfo(SUNSET_STAT_MAC_NO_REPLY, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");	
			}
			
			break;
			
		case SUNSET_MAC_ACTION_PKT_DISCARDED:
			
			if (Sunset_Statistics::use_stat() && stat != NULL) {
				stat->logStatInfo(SUNSET_STAT_MAC_DISCARD, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");
			}
			
			break;
			
		default:
			
			break;
	}
}

/*! @brief Function called when a reception operation is performed to check if an action has to be taken
 *	@param[in] p   The packet the action is related to.
 *	@param[in] mct The reception action that has been executed.
 */

void Sunset_Mac::rxAction(Packet* p, mac_action_type mct){
	
	switch (mct) {
		
		case SUNSET_MAC_RX_ACTION_OK:
			
			if (Sunset_Statistics::use_stat() && stat != NULL) {
				
				stat->logStatInfo(SUNSET_STAT_MAC_RX, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");
				
			}
			
			break;
			
		case SUNSET_MAC_RX_ACTION_DONE:
			
			break;
			
		case SUNSET_MAC_RX_ACTION_ERROR:
			
			if (Sunset_Statistics::use_stat() && stat != NULL) {
				
				stat->logStatInfo(SUNSET_STAT_MAC_RX_ERROR, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");	
				
			}
			
			break;
			
		default:
			break;
	}
}
