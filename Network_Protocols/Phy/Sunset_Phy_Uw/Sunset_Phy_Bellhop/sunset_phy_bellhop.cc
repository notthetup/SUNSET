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

#include "sunset_phy_bellhop.h"
#include <sunset_phy2mac-clmsg.h>
#include <phymac-clmsg.h>
#include <sunset_utilities.h> 

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_Phy_BellhopClass : public TclClass {
public:
	
	Sunset_Phy_BellhopClass() : TclClass("Module/Sunset_Phy_Bellhop") {}
	
	TclObject* create(int argc, const char*const* argv) {
		
		return (new Sunset_Phy_Bellhop());
	}
	
} class_Sunset_Phy_Bellhop;

Sunset_Phy_Bellhop::Sunset_Phy_Bellhop()
{
	
	use_pkt_error = 0;
	use_energy = 0;
	
	bind("use_pkt_error_", &use_pkt_error);
	bind("use_energy_", &use_energy);
	
	energy = NULL;
	pkt_error = NULL;
	
	sid = NULL;
	
	sid_id = 0;
	
	Sunset_Debug::debugInfo(-1, -1, "Sunset_Phy_Bellhop::Sunset_Phy_Bellhop CREATED");
	
}

Sunset_Phy_Bellhop::~Sunset_Phy_Bellhop() 
{	
}

/*!
 * 	@brief The start() function can be called from the TCL script to execute bellhop module operations when the simulation/emulation starts.
 */

void Sunset_Phy_Bellhop::start()
{
	Sunset_Module::start();
	
	state = IDLE;
	
	startRx_ = endRx_ = 0;
	
	startIdle_ = NOW;
	
	sid = Sunset_Information_Dispatcher::instance();
	
	sid_id = sid->register_module(phyAddress, "PHY_BELLHOP", this);
	
	lastPower = 0.0;
	
	if (sid != NULL) {
		
		sid->define(phyAddress, sid_id, "TX_POWER");
		sid->subscribe(phyAddress, sid_id, "TX_POWER");
	}	
	
	if ( use_pkt_error ) {
	
		pkt_error = (Sunset_Packet_Error_Model::instance());
	}

	return;
}

/*!
 * 	@brief The stop() function can be called from the TCL script to execute bellhop module operations when the simulation/emulation stops.
 */

void Sunset_Phy_Bellhop::stop()
{
	
	Sunset_Module::stop();
	
	double duration = 0;
	double power = 0.0;
	
	Sunset_Debug::debugInfo(3, phyAddress, "Sunset_Phy_Bellhop::end - STATE %d", state);
	
	if ( use_energy ) {
		
		if ( state == IDLE ) {
			
			duration = NOW - startIdle_;
			
			if ( use_energy ) {
				
				energy->setIdleDuration(duration);
			}
			
		}
		else if ( state == START_TX ) {
			
			duration = NOW - startTx_;
			
			lastPower = round(lastPower * 100) / 100.0;
			
			if ( lvlPowers.find(lastPower) != lvlPowers.end() ) {
				
				energy->setTxDuration(lastPower, duration);
				
			} else {
				
				Sunset_Debug::debugInfo(-1, phyAddress, "Sunset_Phy_Bellhop::end ERROR Energy %f", power);
				exit(0);	
			}	
		} 
		else if ( state == START_RX ) {
			
			duration = NOW - startRx_;
			
			energy->setRxDuration(duration);
		}
		else {
			Sunset_Debug::debugInfo(-1, phyAddress, "Sunset_Phy_Bellhop::end ERROR - STATE %d", state);
			exit(0);
		}
	}
	
	sid_id = 0;
	
	sid = NULL; 
	
	pkt_error = NULL;
	
	return;
}



/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script 
 *	@param argc argc is a count of the arguments supplied to the command function.
 *	@param argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_Phy_Bellhop::command(int argc, const char * const * argv ) 
{
	
	if (argc == 2) {
		
		/* The "start" command starts the Bellhop PHY module */
		
		if (strcmp(argv[1], "start") == 0) {
			
			Sunset_Debug::debugInfo(1, phyAddress, "Sunset_Phy_Bellhop::command start Module");
			start();
			
			return TCL_OK;
		}
		
		/* The "stop" command stops the Bellhop PHY module */
		
		if (strcmp(argv[1], "stop") == 0) {
			
			Sunset_Debug::debugInfo(1, phyAddress, "Sunset_Phy_Bellhop::command stop Module");
			stop();
			
			return TCL_OK;
		}
	}
	
	if( argc == 3 ) {
		
		/* If "usePktErrorModel" is set then the SUNSET packet error model will be used. */
		
		if (strcmp(argv[1], "usePktErrorModel") == 0) {
			
			use_pkt_error = atoi(argv[2]);
			
			Sunset_Debug::debugInfo(3, phyAddress, "Sunset_Phy_Bellhop::command usePktErrorModel %d", use_pkt_error);
			
			return (TCL_OK);
		}
		
		/* The "setModuleAddress" command sets the address of the PHY module. */
		
		if(strcasecmp(argv[1], "setModuleAddress") == 0) {
			
			phyAddress = atoi(argv[2]);
			
			Sunset_Debug::debugInfo(3, -1, "Sunset_Phy_Bellhop::command phyAddress %d", phyAddress);
			
			return TCL_OK;
		}
		
		/* The "addEnergyModule" command adds Sunset_Energy_Model to the PHY module. */
		
		if(strcasecmp(argv[1], "addEnergyModule") == 0) {
			
			energy = (Sunset_Energy_Model*) TclObject::lookup(argv[2]);
			use_energy = 1;
			
			return TCL_OK;
		}
		
		/* The "addPowerLevel" command adds a transmission power to the PHY module. */
		
		if(strcasecmp(argv[1], "addPower") == 0) {
			
			lvlPowers.insert(atof(argv[2]));
			
			Sunset_Debug::debugInfo(3, -1, "Sunset_Phy_Bellhop::command addLevelPower %f", atof(argv[2]));
			
			return TCL_OK;
			
		}
	}
	
	return WossMPhyBpsk::command( argc, argv );
}

/*!
 * 	@brief The startTx function sends a packet to the device attached at the lower layer.
 *	@param p The packet to be sent.
 */

void Sunset_Phy_Bellhop::startTx(Packet* p) {
	
	double duration = 0;
	
	if ( use_energy ) {
		
		if ( state == IDLE ) {
			
			duration = NOW - startIdle_;
			
			energy->setIdleDuration(duration);
			
		}
		else if ( state == START_RX ) {
			
			duration = NOW - startRx_;
			
			energy->setRxDuration(duration);
		}
		else {
			Sunset_Debug::debugInfo(-1, phyAddress, "Sunset_Phy_Bellhop::startTx ERROR - STATE %d", state);
			exit(0);		
		}
		
		lastPower = 10.0*log10(HDR_MPHY(p)->Pt);
		lastPower = round(lastPower * 100) / 100.0;
	}
	
	state = START_TX;
	
	startTx_ = NOW;
	
	return WossMPhyBpsk::startTx(p);
}

/*!
 * 	@brief The endTx function is invoked when the data transmission is completed.
 *	@param p The transmitted packet.
 */

void Sunset_Phy_Bellhop::endTx(Packet* p) {
	
	double duration = 0.0;
	double power = 0.0;
	
	Sunset_Debug::debugInfo(3, phyAddress, "Sunset_Phy_Bellhop::endTx - STATE %d", state);
	
	if ( use_energy ) {
		
		if ( state == START_TX ) {
			
			duration = NOW - startTx_;
			
			power = 10.0*log10(HDR_MPHY(p)->Pt);
			
			power = round(power * 100) / 100.0;
						
			Sunset_Debug::debugInfo(3, phyAddress, "Sunset_Phy_Bellhop::endTx - pow %f", power);		
			
			if ( lvlPowers.find(power) != lvlPowers.end() ) {
				
				energy->setTxDuration(power, duration);
				
			} else {
				
				Sunset_Debug::debugInfo(-1, phyAddress, "Sunset_Phy_Bellhop::endTx ERROR Energy %f", power);
				exit(0);	
			}
		}
	}
	
	state = IDLE;
	startIdle_ = NOW;
	
	return WossMPhyBpsk::endTx(p);
}

/*!
 * 	@brief The startRx function is invoked when the start-of-PHY-reception event is detected. It informs the MAC about it.
 *	@param p The packet under reception. According to the specific device attached at the channel layer.
 */

void Sunset_Phy_Bellhop::startRx(Packet* p) {
	
	double duration = 0.0;
	double power = 0.0;
	
	Sunset_Debug::debugInfo(3, phyAddress, "Sunset_Phy_Bellhop::startRx - STATE %d", state);
	
	if ( state == IDLE ) {
		
		duration = NOW - startIdle_;
		
		if ( use_energy ) {
			
			energy->setIdleDuration(duration);
		}
		
		startRx_ = NOW;
		endRx_ = 0.0;
		
		endRx_ = NOW+HDR_MPHY(p)->duration;
		
	} else if ( state == START_TX ) {
		
		duration = NOW - startTx_;
		
		if ( use_energy ) {
			
			power = 10.0*log10(HDR_MPHY(p)->Pt);
			
			power = round(power * 100) / 100.0;
						
			if ( lvlPowers.find(power) != lvlPowers.end() ) {
							
				energy->setTxDuration(power, duration);
				
			} else {
				
				Sunset_Debug::debugInfo(-1, phyAddress, "Sunset_Phy_Bellhop::startRx ERROR Energy %f", power);
				exit(0);	
			}
		}
		
		startRx_ = NOW;
		endRx_ = 0.0;
		
		endRx_ = NOW+HDR_MPHY(p)->duration;
		
	} else if ( state == START_RX ) {
		
		if ( state == START_RX ) {
			
			if ( HDR_MPHY(p)->duration > endRx_ ) {
				
				endRx_ = NOW + HDR_MPHY(p)->duration;
				
				Sunset_Debug::debugInfo(3, phyAddress, "Sunset_Phy_Bellhop::startRx - STATE %d -%f", state, endRx_);
			}
		}
		
	} else {
		
		Sunset_Debug::debugInfo(-1, phyAddress, "Sunset_Phy_Bellhop::startRx ERROR - STATE %d", state);
		exit(0);
	}
	
	state = START_RX;
	
	return WossMPhyBpsk::startRx(p);  
}

/*!
 * 	@brief The endRx function is invoked when the end-of-PHY-reception event is detected. It then sends the received packet to the upper layers (MAC).
 *	@param p The received packet.
 */

void Sunset_Phy_Bellhop::endRx(Packet* p) 
{
	double duration = 0;
	
	if ( state == START_RX && NOW == endRx_ ) {
		
		duration = NOW - startRx_;
		
		if ( use_energy ) {
			
			energy->setRxDuration(duration);
		}
		
		state = IDLE;
		startIdle_ = NOW;
	}
	
	if ( use_pkt_error && pkt_error != NULL ) {
					
		if (PktRx != 0) {
			
			if (PktRx == p) {  
				
				hdr_cmn* ch = HDR_CMN(p);
				hdr_MPhy* ph = HDR_MPHY(p);
				
				double per;
				int nbits;
				bool err;
				double x;
				
				nbits = ch->size()*8;
				x = RNG::defaultrng()->uniform_double();
				
				per = pkt_error->getPER(ph->Pr/ph->Pn, nbits);
				
				err = x <= per;
				
				ch->error() = err;
				
				if (err) {
					
					incrErrorPktsNoise();
				}
				
				sendUp(p);
				
				PktRx = 0; // We can now sync onto another packet
				
			} else {
				
				dropPacket(p);
			}
			
		} else {
			
			dropPacket(p);
		}
		
		return;
	}
	
	return WossMPhyBpsk::endRx(p);
}

double Sunset_Phy_Bellhop::getNoiseChannel()
{
	assert(propagation_);
	
	double freq = spectralmask_->getFreq();
	double bw = spectralmask_->getBandwidth();
	
	UnderwaterMPropagation* uwmp = dynamic_cast<UnderwaterMPropagation*>(propagation_);
	assert(uwmp);
	
	double noiseSPDdBperHz = uwmp->uw.getNoise(freq/1000.0);
	double noisepow = bw * pow(10, noiseSPDdBperHz/10.0);
  	
	Sunset_Debug::debugInfo(5, phyAddress, "Sunset_Phy_Bellhop::getNoiseChannel - freq %f - bw %f - Pn %f", freq, bw, noisepow);
	
	return (noisepow);
}

int Sunset_Phy_Bellhop::notify_info(list<notified_info> linfo) 
{ 
	list<notified_info>::iterator it = linfo.begin();
	notified_info ni;
	
	for (; it != linfo.end(); it++) {
		
		ni = *it;
		string s = "TX_POWER";
				
		if ( ni.node_id == phyAddress ) {
			
			if (strncmp((ni.info_name).c_str(), s.c_str(), strlen(s.c_str())) == 0 ) {
				
// 				MaxTxSPL_dB_ =  *static_cast<float *>(ni.info_value);
				
				sid->get_value(&MaxTxSPL_dB_, ni);
				
				Sunset_Debug::debugInfo(3, phyAddress, " Sunset_Phy_Bellhop::notify_info Power Set to %f", MaxTxSPL_dB_);
				
				return 1;
			}
		}
	}
	
	return 0; 
} 

