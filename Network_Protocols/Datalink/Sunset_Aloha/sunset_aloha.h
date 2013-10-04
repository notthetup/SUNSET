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



#ifndef __Sunset_Aloha_h__
#define __Sunset_Aloha_h__

#include <sunset_queue.h>
#include <sunset_utilities.h>
#include <sunset_timing.h>
#include <sunset_mac_pkt.h>
#include <sunset_debug.h>
#include <sunset_address.h>
#include <sunset_module.h>
#include <sunset_mac.h>


/*! @brief This class implements the Aloha protocol. It extends the Sunset_Mac class. 
 * @see class Sunset_Mac
 */


class Sunset_Aloha : public Sunset_Mac {
	
	friend class Sunset_Aloha_Timer;	
	
public: 
	Sunset_Aloha();
	
protected:
	
	/*! @brief Function called when the node starts the operation to transmit packet p. */
	virtual void send(Packet* p);
	
	/*! @brief Function called if there are packets waiting to be transmitted. */
	virtual void anotherTransmission();
	
	virtual void start();	
	virtual void stop();
	
	/*! @brief Function called when a transmission operation is perfomed to check if an action has to be taken*/
	virtual void txAction(Packet* p, mac_action_type mct);
	
	/*! @brief Function called when a reception operation is perfomed to check if an action has to be taken*/
	virtual void rxAction(Packet* p, mac_action_type mct);
	
	virtual void Phy2MacEndTx(const Packet* p);
	virtual void Phy2MacTxAborted(const Packet* p);

};

#endif
