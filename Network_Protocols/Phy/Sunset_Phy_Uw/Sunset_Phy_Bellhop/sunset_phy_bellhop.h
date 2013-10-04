/* SUNSET - Sapienza University Networking framework for underwater Simulation, Emulation and real-life Testing
 *
 * Copyright (C) 2012 Regents of UWSN Group of SENSES Lab <http://reti.dsi.uniroma1.it/SENSES_lab/>
 *
 * Author: Daniele Spaccini - spaccini@di.uniroma1.it
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
 * A special thanks go to Roberto Petroccia <petroccia@di.uniroma1.it> which has contributed to the development of this module. 
 */


#ifndef __Sunset_Phy_Bellhop_h__
#define __Sunset_Phy_Bellhop_h__

#include <module.h>
#include <node-core.h>
#include <map>
#include <set>
#include <queue>
#include <stdlib.h>
#include <sunset_energy_model.h>
#include <interference_miv.h>
#include <underwater-mpropagation.h>
#include <sunset_module.h>
#include <sunset_debug.h>
#include <channel-module.h>
#include <uw-woss-bpsk.h>
#include <phymac-clmsg.h>
#include <underwater-bpsk.h>
#include <rng.h>
#include <sunset_information_dispatcher.h>

#include <cassert>
#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <sstream>
#include <string.h>
#include <cmath>
#include <limits>
#include <climits>

#include <sunset_packet_error_model.h>

enum { IDLE = 0, START_TX = 1, END_TX = 2, START_RX = 3, END_RX = 4};

/*! @brief This class extends the WossMPhyBpsk ("Bellhop" racy tracing model) class of WOSS enabling the usage of several transmission powers and energy consumption models.
 */

class Sunset_Phy_Bellhop: public WossMPhyBpsk, public Sunset_Module {
	
private:
	int phyAddress;
	int state;
        double startRx_, endRx_;
        double startIdle_, startTx_;
	
	Sunset_Energy_Model *energy;
	Sunset_Packet_Error_Model *pkt_error;
	
	set<double> lvlPowers;
	
	Sunset_Information_Dispatcher* sid;
	int sid_id;
	
	double lastPower;
	
public:
	Sunset_Phy_Bellhop();
	~Sunset_Phy_Bellhop();
 	
	virtual double getNoiseChannel();	
	virtual int command(int argc, const char* const* argv);
	
	virtual void startTx(Packet* p);
        virtual void endTx(Packet* p);
	
        virtual void startRx(Packet* p);
        virtual void endRx(Packet* p);
	
        virtual void start();
        virtual void stop();
	virtual int notify_info(list<notified_info> linfo);
		
	int use_pkt_error;
	int use_energy;
	
};

#endif