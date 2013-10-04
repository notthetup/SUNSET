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
 * The author would like to thank Roberto Petroccia <petroccia@di.uniroma1.it>, which has contributed to the development of this module. 
 */

#include "sunset_energy_model.h"

static class Sunset_Energy_ModelClass : public TclClass {
public:
        
	Sunset_Energy_ModelClass() : TclClass("Module/Sunset_Energy_Model") {}
        
        TclObject* create(int, const char*const*) {
		
		return (new Sunset_Energy_Model);
        }
        
} class_Sunset_Energy_Model;

Sunset_Energy_Model::Sunset_Energy_Model()
{
	// initialize nergy module variables
        totalEnergy = 0.0;
        rxTime = idleTime = 0.0;
	rxPower = idlePower = 0.0;
	
	stat = NULL;
	
	Sunset_Debug::debugInfo(3, -1, "Sunset_Energy_Model::Sunset_Energy_Model CREATED");
}

/*!
 * 	@brief The start() function can be called from the TCL script to execute energy model initializations when the simulation/emulation starts.
 */

void Sunset_Energy_Model::start()
{
	Sunset_Module::start();
	
	rxTime = idleTime = 0.0;
	
	// create the reference to the statistics module
	if (Sunset_Statistics::use_stat()) {
		stat = Sunset_Statistics::instance();   
	}
	
	// check if power consumptions for tx, rx and idle have been set
	if ( txPower.size() == 0 ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Energy_Model::start() No TX Consumption set!");
		exit(-1);
	}
	
	if ( rxPower == 0.0 ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Energy_Model::start() No RX Consumption set!");
		exit(-1);
	}
	
	if ( idlePower == 0.0 ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Energy_Model::start() No IDLE Consumption set!");
		exit(-1);
	}
	
	return;
}

/*!
 * 	@brief The stop() function can be called from the TCL script to execute energy model operations when the simulation/emulation stops.
 */

void Sunset_Energy_Model::stop()
{
	Sunset_Module::stop();
	
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

int Sunset_Energy_Model::command(int argc, const char*const* argv)
{
	
        Tcl& tcl = Tcl::instance();
	
        if ( argc == 2 ) {
		
		/* The "stop" command stops the Energy Model */
		
                if( strcasecmp(argv[1], "stop") == 0 ) {
                        stop();
                        return TCL_OK;
                }
                
                /* The "start" command starts the Energy Model */
                
                if( strcasecmp(argv[1], "start") == 0 ) {
                        start();
                        return TCL_OK;
                }
        }
	
        if( argc == 3) {
		
		/* The "setInitialEnergy" command sets the maximal energy */
		
                if( strcasecmp(argv[1], "setInitialEnergy") == 0 )
                {
                        totalEnergy = atof(argv[2]);
			residualEnergy = totalEnergy;
			
			Sunset_Debug::debugInfo(3, -1, "Sunset_Energy_Model::command totalEnergy %f", totalEnergy);
			
			return TCL_OK;
                }
                
                /* The "setRxPower" command sets the power consumption in reception */
                
                if( strcasecmp(argv[1], "setRxPower") == 0 )
                {
                        rxPower = atof(argv[2]);
			
			Sunset_Debug::debugInfo(3, -1, "Sunset_Energy_Model::command rxPower %f", rxPower);
			
                        return TCL_OK;
                } 
                
                /* The "setIdlePower" command sets the power consumption in idle */
                
                if( strcasecmp(argv[1], "setIdlePower") == 0 )
                {
                        idlePower = atof(argv[2]);
			
			Sunset_Debug::debugInfo(3, -1, "Sunset_Energy_Model::command idlePower %f", idlePower);
			
                        return TCL_OK;
                } 
                
		/* The "setTxPower" command sets the ideal power consumption in Watt for a given transmission power in dB re micor Pa*/
		
		if( strcasecmp(argv[1], "setTxPower") == 0 )
                {
                        txPower[atof(argv[2])] = fromuPaToWatt(atof(argv[2]));
			
			txTime[atof(argv[2])] = 0.0;
			
			Sunset_Debug::debugInfo(3, -1, "Sunset_Energy_Model::command setTxConsumption %f -> %f", atof(argv[2]), fromuPaToWatt(atof(argv[2])));
			
                        return TCL_OK;
                }
		
		/* The "setModuleAddress" command sets the address of the Energy Model module. */
		
                if(strcasecmp(argv[1], "setModuleAddress") == 0) {
			
			eneAddress = atoi(argv[2]);
			
			Sunset_Debug::debugInfo(3, -1, "Sunset_Energy_Model::command eneAddress %d", getModuleAddress());
			
			return TCL_OK;
		}
        }
        else if ( argc == 4 ) {
		
		/* The "setTxPower" command sets the power consumption in Watt for a given transmission power in dB re micor Pa*/
		
		if( strcasecmp(argv[1], "setTxPower") == 0 )
                {
                        txPower[atof(argv[2])] = atof(argv[3]);
			
			txTime[atof(argv[2])] = 0.0;
			
			Sunset_Debug::debugInfo(3, -1, "Sunset_Energy_Model::command setTxConsumption %f -> %f", atof(argv[2]), atof(argv[3]));
			
                        return TCL_OK;
                }
        }
        
        // Statistics methods
	
	if( argc == 2) {
		
		/* The "getResidualEnergy" returns the residual energy of the node. */
		
		if (strcmp(argv[1], "getResidualEnergy") == 0) {
			
			tcl.resultf("%f", getResidualEnergy());
			
			return TCL_OK;
		}
		
		/* The "getIdleTime" returns the time (in sec.) spent by the node in idle. */
		
		if (strcmp(argv[1], "getIdleTime") == 0) {
			
			tcl.resultf("%f", getIdleTime());
			
			return TCL_OK;
		}
		
		/* The "getRxTime" returns the time (in sec.) spent by the node in receiving mode. */
		
		if (strcmp(argv[1], "getRxTime") == 0) {
			
			tcl.resultf("%f", getRxTime());
			
			return TCL_OK;
		}
		
		/* The "getTotTxTime" returns the time (in sec.) spent by the node while transmitting. */
		
		if (strcmp(argv[1], "getTotTxTime") == 0) {
			
			tcl.resultf("%f", getTotTxTime());
			
			return TCL_OK;
		}
		
		/* The "getIdleConsumption" returns energy consumed by the node in idle. */
		
		if (strcmp(argv[1], "getIdleConsumption") == 0) {
			
			tcl.resultf("%f", getIdleConsumption());
			
			return TCL_OK;
		}
		
		/* The "getRxConsumption" returns the energy consumed by the node in receiving mode. */
		
		if (strcmp(argv[1], "getRxConsumption") == 0) {
			
			tcl.resultf("%f", getRxConsumption());
			
			return TCL_OK;
		}
		
		/* The "getTotTxConsumption" returns the energy consumed by the node while transmitting. */
		
		if (strcmp(argv[1], "getTotTxConsumption") == 0) {
			
			tcl.resultf("%f", getTotTxConsumption());
			
			return TCL_OK;
		}
	}
	
	if ( argc == 3 ) {
		
		/* The "getTxTime" returns the time (in sec.) spent by the node while transmitting using a given tx power. */
		
		if (strcmp(argv[1], "getTxTime") == 0) {
			
			tcl.resultf("%f", getTxTime(atof(argv[2])));
			
			return TCL_OK;
		}
		
		/* The "getTxConsumption" returns energy power by the node while transmitting using a given tx power. */
		
		if (strcmp(argv[1], "getTxConsumption") == 0) {
			
			tcl.resultf("%f", getTxConsumption(atof(argv[2])));
			
			return TCL_OK;
		}
	}
	
        return TclObject::command(argc, argv);
}

/*! 	@brief The setTxDuration() function sets the energy consumed transmitting at the power "pow" for "sec" seconds.
 * 	@param pow The transmission power.
 * 	@param sec Transmission time expressed in seconds.
 */

void Sunset_Energy_Model::setTxDuration(double pow, double sec) 
{ 
	txTime[pow] += sec; 
	
	residualEnergy = residualEnergy - (txPower[pow] * sec);
	
	Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Energy_Model::setTxDuration Sec %f - Tot Sec %f - Residual Energy %f", sec, txTime[pow], residualEnergy);
	
	if (Sunset_Statistics::use_stat() && stat != NULL) {
		
		stat->logStatInfo(SUNSET_STAT_ENERGY_TX, getModuleAddress(), NULL, 0.0, "%f %f %f", txPower[pow], pow, sec);
	}
	
	return; 
}

/*! 	@brief The setRxDuration() function sets the energy consumed receiving for "sec" seconds.
 * 	@param sec Receiving time expressed in seconds.
 */

void Sunset_Energy_Model::setRxDuration(double sec) 
{ 
	rxTime += sec; 
	
	residualEnergy = residualEnergy - (rxPower * sec);
	
	Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Energy_Model::setRxDuration Sec %f - Tot Sec %f - Residual Energy %f", sec, rxTime, residualEnergy);
	
	if (Sunset_Statistics::use_stat() && stat != NULL) {
		
		stat->logStatInfo(SUNSET_STAT_ENERGY_RX, getModuleAddress(), NULL, 0.0, "%f %f", rxPower, sec);
	}
	
	return; 
}

/*! 	@brief The setIdleDuration() function sets the energy consumed idling for "sec" seconds.
 * 	@param sec Idle time expressed in seconds.
 */

void Sunset_Energy_Model::setIdleDuration(double sec) 
{ 
	idleTime += sec; 
	
	residualEnergy = residualEnergy - (idlePower * sec);
	
	Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Energy_Model::setIdleDuration Sec %f - Tot Sec %f - Residual Energy %f", sec, idleTime, residualEnergy);
	
	if (Sunset_Statistics::use_stat() && stat != NULL) {
		
		stat->logStatInfo(SUNSET_STAT_ENERGY_IDLE, getModuleAddress(), NULL, 0.0, "%f %f", idlePower, sec);
	}
	
	return; 
}

/*! 	@brief The fromuPaToWatt() function returns the Watt consumption related to the transmission power pw (expressed in uPa).
 * 	@param pw Transmission power in uPa.
 * 	@retval Power expressed in Watt
 */

double Sunset_Energy_Model::fromuPaToWatt(double pw)
{
	// same function of WOSS (underwater-bpsk.cc)
	
	// Parameters originally intended to be for the WHOI modem  
	const double uPa_to_W  =  6.3096e-18; // -172 dB
	const double TX_efficiency = 0.25;  

	double power = pow(10.0, pw/10.0) * uPa_to_W / TX_efficiency; // in Watt

	return power;
}

/*! 	@brief The getResidualEnergy() function returns the residual energy of the node.
 *	@retval Residual energy. 
 */

float Sunset_Energy_Model::getResidualEnergy() 
{ 
	return residualEnergy; 
}

/*! 	@brief The getIdleTime() function returns the time spent by the node in idle.
 *	@retval Idle time expressed in seconds. 
 */

float Sunset_Energy_Model::getIdleTime() 
{ 
	return idleTime;
}

/*! 	@brief The getRxTime() function returns the time spent by the node while receiving.
 *	@retval Receiving time expressed in seconds. 
 */

float Sunset_Energy_Model::getRxTime() 
{ 
	return rxTime; 
}

/*! 	@brief The getTotTxTime() function returns the time spent by the node while transmitting.
 *	@retval Transmitting time expressed in seconds. 
 */

float Sunset_Energy_Model::getTotTxTime() 
{ 
	map<double, double>::iterator it;
	double sum = 0.0;
	
	for ( it = txTime.begin(); it != txTime.end(); it++ ) {
		
		sum += (*it).second;
	}
	
	return sum; 
}

/*! 	@brief The getTxTime() function returns the time spent by the node while transmitting using a given transmission power.
 * 	@param pow The transmission power we are interested in.
 *	@retval Transmitting time using the given transmission power expressed in seconds. 
 */

float Sunset_Energy_Model::getTxTime(double pow) 
{ 
	if ( txTime.find(pow) == txTime.end() ) {
	
		return -1;
	}
	
	return txTime[pow]; 
}

/*! 	@brief The getIdleConsumption() function returns the energy consumed by the node in idle mode.
 *	@retval Energy consumed expressed in Joule. 
 */

float Sunset_Energy_Model::getIdleConsumption()
{
	return idleTime * idlePower;
}

/*! 	@brief The getRxConsumption() function returns the energy consumed by the node in receiving mode.
 *	@retval Energy consumed expressed in Joule. 
 */

float Sunset_Energy_Model::getRxConsumption()
{
	return rxTime * rxPower;
}

/*! 	@brief The getTotTxConsumption() function returns the total energy consumed by the node while transmitting.
 *	@retval Energy consumed expressed in Joule. 
 */

float Sunset_Energy_Model::getTotTxConsumption()
{
	map<double, double>::iterator it;
	double sum = 0.0;
	double temp = 0.0;
	
	for ( it = txPower.begin(); it != txPower.end(); it++ ) {

		if ( txTime.find((*it).first) == txTime.end() ) {
			
			return -1;
		}

		temp = getTxTime((*it).first) * (*it).second;
		sum += temp;
	}
	
	return sum; 
}

/*! 	@brief The getTxTime() function returns the energy consumed by the node while transmitting using a given transmission power.
 * 	@param pow The transmission power we are interested in.
 *	@retval Energy consumed using the given transmission power expressed in Joule. 
 */

float Sunset_Energy_Model::getTxConsumption(double pow)
{
	if ( txTime.find(pow) == txTime.end() || txPower.find(pow) == txPower.end() ) {
	
		return -1;
	}
	
	return txTime[pow] * txPower[pow];
}
