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

#ifndef __Sunset_Energy_Model_h__
#define __Sunset_Energy_Model_h__

#include <map>
#include <set>
#include <list>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <module.h>
#include <node-core.h>
#include <math.h>
#include <assert.h>
#include <module.h>
#include <node-core.h>
#include <sunset_module.h>
#include <sunset_statistics.h>

/*! @brief This class defines an energy model that can be used by the SUNSET modules to estimate the node energy consumption. Different energy consumptions can be set according to different transmission powers used by the network protocols. */

class Sunset_Energy_Model: public Sunset_Module, public TclObject {
	
public:
	
        Sunset_Energy_Model();
        ~Sunset_Energy_Model() { };
	
        virtual int command(int argc, const char* const* argv);
	
	void setTxDuration(double, double);
	void setRxDuration(double );
	void setIdleDuration(double );
	
        virtual void stop();
	virtual void start();
	
	float getResidualEnergy();
	
private:
	
	int getModuleAddress() { return eneAddress; }
	
        double residualEnergy; 
	int eneAddress;
	
	map<double, double> txPower; 	/*!< \brief Energy consumed transmitting at given transmission power */
	double rxPower;			/*!< \brief Energy consumed in reception */
	double idlePower;		/*!< \brief Energy consumed in idle */
	
	map<double, double> txTime;  	/*!< \brief Number of seconds spent transmitting at given transmission power */
	double rxTime;			/*!< \brief Number of seconds spent in reception */
	double idleTime;		/*!< \brief Number of seconds spent in idle */
	
        double totalEnergy;
	
	// These functions compute basic statistic information to be able to provide a feedback to the user even if the statistic module is not used
	//------------------------------------
	float getIdleTime();
	float getRxTime();
	float getTxTime(double );
	float getTotTxTime();
	
	float getIdleConsumption();
	float getRxConsumption();
	float getTxConsumption(double );
	float getTotTxConsumption();
	//------------------------------------
	
	double fromuPaToWatt(double );
	
	Sunset_Statistics* stat;
};

#endif
