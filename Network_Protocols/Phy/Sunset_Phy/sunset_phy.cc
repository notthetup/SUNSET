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


#include<rng.h>
#include<iostream>

#include "sunset_phy.h"
#include "sunset_modem2phy-clmsg.h"
#include "sunset_debug.h"
#include "sunset_phy2mac-clmsg.h"
#include <sunset_utilities.h>

#include "phymac-clmsg.h"

extern ClMessage_t CLMSG_MODEM2PHY_TXABORTED;
extern ClMessage_t CLMSG_MODEM2PHY_ENDTX;
extern ClMessage_t CLMSG_MODEM2PHY_STARTRX;

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_PhyClass : public TclClass 
{
public:
	Sunset_PhyClass() : TclClass("Module/MPhy/Sunset_Phy") {}
	
	TclObject* create(int, const char*const*) {
		
		return (new Sunset_Phy);
	}
	
} class_Sunset_Phy;


Sunset_Phy::Sunset_Phy() : MPhy(), PktRx(0), txPending(false), txBusy_(this)
{
	use_pkt_error = 0;
	
	// Get variables initialization from the Tcl script
	
	bind("moduleAddress", &module_address);
	
	bind("use_pkt_error_", &use_pkt_error);
	
	bind("use_ch_emu", &use_ch_emu);
	
	pkt_error = NULL;
	
}

/*!
 * 	@brief The start() function can be called from the TCL scripts to execute phy module operations when the simulation/emulation starts.
 */

void Sunset_Phy::start() 
{
	Sunset_Module::start(); // call father's start function
	
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Phy::start");
	
	if ( use_pkt_error == 1 ) {
		
		pkt_error = (Sunset_Packet_Error_Model::instance());
	}
}

/*!
 * 	@brief The stop() function can be called from the TCL scripts to execute phy module operations when the simulation/emulation stops.
 */

void Sunset_Phy::stop() 
{
	Sunset_Module::stop(); //call father's stop function
	
	pkt_error = NULL;
	
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Phy::stop");
	
}

/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script 
 *	@param[in] argc argc is a count of the arguments supplied to the command function.
 *	@param[in] argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_Phy::command(int argc, const char*const* argv) 
{
	if ( argc == 2 ) {
		
		/* The "start" command starts the PHY module */
		
		if (strcmp(argv[1], "start") == 0) {
			
			start();
			
			return TCL_OK;
		}
		
		/* The "stop" command stops the PHY module */
		
		if (strcmp(argv[1], "stop") == 0) {
			
			stop();
			
			return TCL_OK;
		}		
	}	
	else if ( argc == 3 ) {
		
		/* The "setModuleAddress" command sets the address of the PHY module. */
		
		if (strcmp(argv[1], "setModuleAddress") == 0) {
			
			module_address = atoi(argv[2]);
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Phy::command getModuleAddress() %d", getModuleAddress());
			
			return (TCL_OK);
		}
		
		/* If "usePktErrorModel" is set to 1 then the SUNSET packet error model will be used. */
		
		if (strcmp(argv[1], "usePktErrorModel") == 0) {
			
			use_pkt_error = atoi(argv[2]);
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Phy::command usePktErrorModel %d", use_pkt_error);
			
			return (TCL_OK);
		}
		
		/* The "addToBlacklist" adds the specified node id to the black list. */
		
		if (strcmp(argv[1], "addToBlacklist") == 0) {
			
			blackList.insert(atoi(argv[2]));
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Phy::command Blacklist %d", atoi(argv[2]));
			
			return (TCL_OK);
		}
		
		/* If "useChEmulator" is set to 1 then the transmission/reception times will be considered. */
		
		if (strcmp(argv[1], "useChEmulator") == 0) {
			
			use_ch_emu = atoi(argv[2]);

			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Phy::command useChEmulator %d", use_ch_emu);
			
			return (TCL_OK);
		}
	}

	return MPhy::command(argc, argv);
}

/*!
 * 	@brief The startTx function sends a packet to the device attached at the lower layer.
 *	@param p The packet to be sent.
 */

void Sunset_Phy::startTx(Packet* p) 
{
	float timeout = 0.0;
	
	if ( use_ch_emu == 1 && ( txBusy_.busy() || rxBusy_.size() != 0) ) {
		
		txAborted(p);
		return;
	}
	
	PktRx = 0;
	txPending = true;
		
	if ( use_ch_emu == 1 ) {
		
		Packet * aux = p->copy();
		txBusy_.p = aux;
		
		timeout = HDR_CMN(p)->txtime();
		
		if (txBusy_.busy()) {
		
			txBusy_.stop();
			
		}
		
		txBusy_.start(timeout);
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Phy::starTx - end transmission at %f p %p aux %p ", Sunset_Utilities::get_now() + timeout, p, aux);

	}

	Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Phy::startTx");
	
	sendDown(p);
}

/*!
 * 	@brief The endTx function is invoked when the data transmission is completed. It informs the MAC about it.
 *	@param p The transmitted packet.
 */

void Sunset_Phy::endTx(Packet* p) 
{
	
	if ( use_ch_emu == 1 && !txBusy_.busy() ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Phy::endTx ERROR");
		
		exit(1);
		return;
	}
	
	if ( use_ch_emu == 0 ) {
	
		txPending = false;
	
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Phy::endTx p %p", p);
		
		Phy2MacEndTx(p);
	}
	
	else {

		if (txBusy_.p != 0) {
		
			SUNSET_HDR_CMN(txBusy_.p)->start_tx_time = SUNSET_HDR_CMN(p)->start_tx_time;
			SUNSET_HDR_CMN(txBusy_.p)->end_tx_time = SUNSET_HDR_CMN(p)->end_tx_time;
			
		}
	}
	
	return;
}

/*!
 * 	@brief The startRx function is invoked when the start-of-PHY-reception event is detected. It informs the MAC about it.
 *	@param p The packet under reception. According to the specific device attached at the channel layer, it could be the actual packet or a place holder.
 */

void Sunset_Phy::startRx(Packet* p) 
{	
	
	if ( p != 0 ) {
		
		if ( blackList.find(SUNSET_HDR_CMN(p)->phy_src) == blackList.end() ) {
		
			PktRx = p;
			
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Phy::startRx");
		}
		else {
			PktRx = p;
						
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Phy::startRx Node %d is in BlackList", SUNSET_HDR_CMN(p)->phy_src );
		}
	}
	else {
		
		Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Phy::startRx");
	}
	
	Phy2MacStartRx(p);
	
}

/*!
 * 	@brief The endRx function is invoked when the end-of-PHY-reception event is detected. It then sends the received packet to the upper layers (MAC).
 *	@param p The received packet.
 */

void Sunset_Phy::endRx(Packet* p) 
{
	
	Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Phy::endRx Node src %d dst %d is in BlackList %d", 
				SUNSET_HDR_CMN(p)->phy_src, SUNSET_HDR_CMN(p)->phy_dst,
				blackList.find(SUNSET_HDR_CMN(p)->phy_src) != blackList.end() );

	if ( blackList.find(SUNSET_HDR_CMN(p)->phy_src) != blackList.end() ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Phy::endRx Node %d is in BlackList", SUNSET_HDR_CMN(p)->phy_src );
		
		HDR_CMN(p)->error() = 1;
		
		sendUp(p);
				
		return;
	}
	
	if ( use_ch_emu == 0 ) {
	
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Phy::endRx use_pkt_error %d - model %d", 
					use_pkt_error, pkt_error != NULL);
		
		if ( use_pkt_error && pkt_error != NULL ) {
			
			double rand = Random::uniform();
			double per = pkt_error->getPER(0.0, HDR_CMN(p)->size() * 8);
			bool error_n = rand <= per;
			
			if ( error_n ) {
				
				Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Phy::endRx PER ERROR %f <= %f", rand, per);
				HDR_CMN(p)->error() = error_n;
			}
		}
		
		sendUp(p);
	}
	else if ( use_ch_emu == 1 ) {
		
		float timeout = 0.0;	
		Sunset_Phy_Rx_Timer *t = new Sunset_Phy_Rx_Timer(this);

		timeout = HDR_CMN(p)->txtime();
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Phy::endRx Timeout %f", timeout);
					
		if ( rxBusy_.size() == 0 ) {
		
			t->id = 0;
			(rxBusy_)[0] = t;
		}
		else {
			
			int id = 0;
			
			id = ((rxBusy_).rbegin())->first;
			id++;
			
			t->id = id;
			
			(rxBusy_)[id] = t;
		}
		
		if ( txBusy_.busy() ) {
			
			// error: i'm receiving while i'm transmitting
			
			HDR_CMN(p)->error() = 1;
		}
		
		t->p = p;
		
		if ( t->busy()) {
			
			t->stop();
			
		}
	
		t->start(timeout);
	}
	
	return;
}

/*!
 * 	@brief The txAborted function is invoked when an uncompleted transmission is detected.
 *	@param p The aborted packet to be notified to the MAC layer.
 */

void Sunset_Phy::txAborted(Packet* p) 
{
	txPending = false;
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Phy::txAborted");
	
	Phy2MacTxAborted(p);
	
}

/*!
 * 	@brief The Phy2MacTxAborted function  is invoked when the abort-of-PHY-reception event is detected.
 *	@param p The aborted packet.
 */

void Sunset_Phy::Phy2MacTxAborted(const Packet* p)
{
	ClMsgPhy2MacTxAborted m(p);
	sendSyncClMsgUp(&m);
}

/*!
 * 	@brief The recv function handles a packet coming from upper or lower layers.
 *	@param p The received packet.
 */

void Sunset_Phy::recv(Packet* p)
{
	
	hdr_cmn *ch = HDR_CMN(p);
	
	if(ch->direction() == hdr_cmn::UP) {
		
		endRx(p);
	}    
	else {
		
		Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Phy::recv DOWN");
		
		startTx(p);		
	}
}

/*!
 * 	@brief The Modem2PhyEndTx function is invoked when the end-of-PHY-transmission event is detected.
 *	@param p The transmitted packet.
 */

void Sunset_Phy::Modem2PhyEndTx(const Packet* p) 
{
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Phy::Modem2PhyEndTx");
	
	endTx((Packet*)p);
}

/*!
 * 	@brief The Modem2PhyTxAborted function is invoked when the abort-of-PHY-transmission event is detected.
 *	@param p The aborted packet.
 */

void Sunset_Phy::Modem2PhyTxAborted(const Packet* p) 
{
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Phy::Modem2PhyTxAborted");
	
	txAborted((Packet*)p);
}

/*!
 * 	@brief The Modem2PhyStartRx function is invoked when the start-of-PHY-reception event is detected.
 *	@param p The packet under reception. According to the specific device attached at the channel layer, it could be the actual packet or a place holder.
 */

void Sunset_Phy::Modem2PhyStartRx(const Packet* p) 
{
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Phy::Modem2PhystartRx");
		
	startRx((Packet*)p);
}

/*!
 * 	@brief The Modem2PhyEndRx function is invoked when the end-of-PHY-reception event is detected.
 *	@param p The received packet.
 */

void Sunset_Phy::Modem2PhyEndRx(Packet* p) 
{
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Phy::Modem2PhyEndRx");
	endRx(p);
}

/*!
 * 	@brief The Phy2recvSyncClMsg function handles the reception of synchronous messages coming from the channel layer, i.e., the real device used for data transmission and reception. 
 *	@param m The synchronous messages coming from the channel layer.
 */

int Sunset_Phy::recvSyncClMsg(ClMessage* m)
{
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Phy::recvSyncClMsg type %d", m->type());
	if (m->type() == CLMSG_MODEM2PHY_TXABORTED) {
		
		Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Phy::recvSyncClMsg TX ABORTED p %p", ((ClMsgModem2PhyTxAborted*)m)->pkt);
		Modem2PhyTxAborted(((ClMsgModem2PhyTxAborted*)m)->pkt);
		
		return 0;
	}
	else if (m->type() == CLMSG_MODEM2PHY_ENDTX) {
		
		Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Phy::recvSyncClMsg END TX p %p", ((ClMsgModem2PhyTxAborted*)m)->pkt);
		Modem2PhyEndTx(((ClMsgModem2PhyEndTx*)m)->pkt);
		
		return 0;
	}
	else if (m->type() == CLMSG_MODEM2PHY_STARTRX) {
		
		Modem2PhyStartRx(((ClMsgModem2PhyStartRx*)m)->pkt);
		
		return 0;
	}
	else if (m->type() == CLMSG_MAC2PHY_GETISIDLE) {
		
		((ClMsgMac2PhyGetIsIdle*)m)->setIsIdle(1);
		
		return 0;
	}
	else {
		return MPhy::recvSyncClMsg(m);
	}
}

/*! @brief Return the packet transmission time. */

double Sunset_Phy::getTxDuration(Packet* p)
{
	return HDR_CMN(p)->txtime();
}

/*! @brief Return the modulation type for the current packet. It is not implemented in the current SUNSET version. */

int Sunset_Phy::getModulationType(Packet* p)
{
	return -1;
}

void Sunset_Phy_Timer::start(double time) 
{
	
	Scheduler& s = Scheduler::instance();

	if (busy_) {

		return;  //TODO: it's just to avoid exiting the simulation
		exit(1);
	}
	
	busy_ = 1;
	paused_ = 0;
	s.sync();
	stime = s.clock();
	rtime = time;
	
	if (rtime < 0.0) {
	
		return;  //TODO: it's just to avoid exiting the simulation
		exit(1);	
	}

	Sunset_Utilities::schedule(this, &intr, rtime);

	return;
}

void Sunset_Phy_Timer::stop(void) 
{
	
	Scheduler& s = Scheduler::instance();
	
	if(paused_ == 0)
		s.cancel(&intr);
	
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;
	
	return;
}

void Sunset_Phy_Tx_Timer::handle(Event *) 
{       
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;
	
	Sunset_Debug::debugInfo(-1, t_->getModuleAddress(), "Sunset_Phy_Tx_Timer::handle");

	(t_)->hTxBusyTimeout(this->p);
}

void Sunset_Phy_Rx_Timer::handle(Event *) 
{       
	busy_ = 0;
	paused_ = 0;
	stime = 0.0;
	rtime = 0.0;
	
	Sunset_Debug::debugInfo(-1, t_->getModuleAddress(), "Sunset_Phy_Rx_Timer::handle");

	(t_)->hRxBusyTimeout(this->p, this->id);
}

void Sunset_Phy::hTxBusyTimeout(Packet *p)
{
	txPending = false;
	
	Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Phy::endTx2 p %p", p);
	
	Phy2MacEndTx(p);
	
	return;
}

void Sunset_Phy::hRxBusyTimeout(Packet *p, int id)
{		
	if ( use_pkt_error && pkt_error != NULL ) {
		
		double rand = Random::uniform();
		double per = pkt_error->getPER(0.0, HDR_CMN(p)->size() * 8);
		bool error_n = rand <= per;
		
		if ( error_n ) {
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Phy::endRx PER ERROR %f <= %f", rand, per);
			HDR_CMN(p)->error() = error_n;
		}
	}
	
	delete rxBusy_[id];
	(rxBusy_).erase(id);

	Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Phy::endRx");

	sendUp(p);
	
	return;
}