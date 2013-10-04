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


#ifndef __Sunset_Phy_h__ 
#define __Sunset_Phy_h__

#include <mphy.h>
#include <map>
#include <sunset_module.h>
#include <sunset_packet_error_model.h>
#include <sunset_common_pkt.h>

class Sunset_Phy;

class Sunset_Phy_Timer : public Handler {
public:
	Sunset_Phy_Timer(Sunset_Phy* t) : t_(t) {
		busy_ = paused_ = 0; stime = rtime = 0.0;
	}
	
	virtual void handle(Event *e) = 0;
	
	virtual void start(double time);
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
	Sunset_Phy *t_;
	
	int		busy_;
	int		paused_;
	Event		intr;
	double		stime;	// start time
	double		rtime;	// remaining time
};

class Sunset_Phy_Tx_Timer : public Sunset_Phy_Timer {
public:
	 Sunset_Phy_Tx_Timer(Sunset_Phy *m) : Sunset_Phy_Timer(m) { }
protected:
	virtual void handle (Event *e);
public:
	Packet *p;
};

class Sunset_Phy_Rx_Timer : public Sunset_Phy_Timer {
public:
	 Sunset_Phy_Rx_Timer(Sunset_Phy *m) : Sunset_Phy_Timer(m) { }
protected:
	virtual void handle (Event *e);
public:
	Packet *p;
	int id;
	
};

/*! \brief This class defines a dummy PHY layer to connect upper layers to the selected device when running in emulation mode. */

class Sunset_Phy : public MPhy, public Sunset_Module {
	
public:
	
	friend class Sunset_Phy_Tx_Timer;
	friend class Sunset_Phy_Rx_Timer;
	
	Sunset_Phy();
	
	virtual int command(int argc, const char* const* argv);
	
	virtual int recvSyncClMsg(ClMessage* m);
	
	virtual void recv(Packet* p);
	
	/*! @brief Return the packet transmission time. */
	virtual double getTxDuration(Packet* p);
	
	/*! @brief Return the modulation type for the current packet */
	virtual int getModulationType(Packet* p);
	
protected:
	
	virtual void start();
	virtual void stop();
	
	/*! @brief Start transmission operations. */
	virtual void startTx(Packet* p);
	
	/*! @brief End transmission operations. */
	virtual void endTx(Packet* p);
	
	/*! @brief Function called when errors occur during packet transmission. */
	virtual void txAborted(Packet* p);
	
	/*! @brief Start reception operations. */
	virtual void startRx(Packet* p);
	
	/*! @brief End transmission operations. */
	virtual void endRx(Packet* p);
	
	void Phy2MacTxAborted(const Packet* p);
	
	// funcitons used for the interaction with the modem layer
	//--------------------------------------
	void Modem2PhyEndTx(const Packet* p);	
	void Modem2PhyTxAborted(const Packet* p);	
	void Modem2PhyStartRx(const Packet* p);	
	void Modem2PhyEndRx(Packet* p);	
	//--------------------------------------
	
private:
	
	/*! @brief Packet the receiver is synchronized to for reception */
	Packet* PktRx;
	
	bool txPending;
	
	Sunset_Packet_Error_Model *pkt_error; // Reference to the packet error model
	
	int use_pkt_error; //1 if the packet error model to simulate probability of error on received packet is used, 0 otherwise
	
	set<int> blackList;
	
	int use_ch_emu; //1 if the transmission/reception times will be considered.
	
	int bitrate;
	int bitsize;
	
	Sunset_Phy_Tx_Timer txBusy_;
	map<int, Sunset_Phy_Rx_Timer *>  rxBusy_; /*!< \brief Timer related to each packet received <id_timer, timer>  */
	
	void hTxBusyTimeout(Packet *);
	void hRxBusyTimeout(Packet *, int);
};

#endif /* SUNSET_PHY_H */



