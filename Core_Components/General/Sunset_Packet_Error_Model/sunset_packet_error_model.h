/* SUNSET - Sapienza University Networking framework for underwater Simulation, Emulation and real-life Testing
 *
 * Copyright (C) 2012 Regents of UWSN Group of SENSES Lab
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



#ifndef __Sunset_Packet_Error_Model_h__
#define __Sunset_Packet_Error_Model_h__

#include <string>
#include <map>
#include <set>
#include <stdlib.h>
#include <sunset_debug.h>
#include <sunset_module.h>

enum error_model { BPSK_MOD = 0, STATIC_MOD = 1, FSK_MOD = 2};

/*! @brief This class defines a packet error model that can be used by the SUNSET modules to obtain the PER related to a certain data packet according to the chosen modulation. Actually the supported modulation are BPSK, FSK or static PER. */

class Sunset_Packet_Error_Model : public TclObject, public Sunset_Module {
	
public:
	static Sunset_Packet_Error_Model *per_model;
	
	Sunset_Packet_Error_Model();
	~Sunset_Packet_Error_Model() {};
	
	virtual int command( int argc, const char*const* argv );
	
	static Sunset_Packet_Error_Model* instance();
    	
	double getPER(double, int ); 
	
	int getErrorModel() { return model; }; /*!< \brief The getErrorModel() return the error model chosen. */
	
private:
	double PER;			/*!< \brief Static PER */
	double RxSnrPenalty_dB_; 	/*!< \brief SNR penalty on the receiver */
	int model;			/*!< \brief Packet error model */
	
	double getPER_bpsk(double, int );
	double getPER_fsk(double, int );
	double getPER_static();
};

#endif
