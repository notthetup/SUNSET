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


#ifndef SUNSET_COMMON_PKT_H
#define SUNSET_COMMON_PKT_H

#include <packet.h>

#define	SUNSET_Version			1.0
#define SUNSET_HDR_CMK_LQ_INIT		-10.0

#define SUNSET_HDR_CMN(P) 		(Sunset_hdr_common::access(P))

extern packet_t PT_SUNSET_COMMON;

/*! @brief The SUNSET common packet header is intended to contain useful information during the simulation/emulation */

typedef struct Sunset_hdr_common 
{
	
	int version;        /*!< \brief  SUNSET FRAMEWORK version */
	
	double rssi;		 /*!< \brief  Measurement on the signal strength */
	
	double link_quality;    /*!< \brief  Measurement on the link quality */

	double doppler;    /*!< \brief  Measurement on the doppler effect */

	double tx_power;    /*!< \brief  Transmission power at the PHY layer for this packet */
	
	double bit_rate;    /*!< \brief  Bit rate to be used for this packet */
	
	int modem_type;     /*!< \brief  What kind of modem has to be used for the transmission if multiple modems are available */
	
	double propagation_delay;   /*!< \brief  Estimated propagation delay between packet source and destination */
	
	double distance;    /*!< \brief  Estimated distance between packet source and destination */
	
	double timestamp;   /*!< \brief  Packet timestamp */
	
	int ranging;        /*!< \brief  Is the packet used to compute ranging information? */
	
	int response;       /*!< \brief  Is the packet carrying response information? */
	
	double start_tx_time;   /*!< \brief  Time when the first byte of data is transmitted (ms accuracy)*/
	
	double end_tx_time;	 /*!< \brief  Time when the last byte of data is transmitted (ms accuracy)*/

	double start_rx_time;	 /*!< \brief  Time when the first byte of data is received (ms accuracy)*/

	double end_rx_time;	 /*!< \brief  Time when the last byte of data is received (ms accuracy)*/

	double processing_delay;	 /*!< \brief  Time between the reception of a request (first byte) and the transmission of a response (first byte) (ms accuracy)*/

	int phy_src;
	
	int phy_dst;
	
	// BACKSEAT STAT
	int run_id;
	int cmd_id;
	
	// Additional fields can be added if needed and can be initialized using the init() function
	
	/* Function to initialize the packet fields */
	
	void init() {
		
		version = (int)SUNSET_Version;
		
		tx_power = -1.0;
		
		bit_rate = -1.0;
		
		modem_type = -1;
		
		propagation_delay = -1.0;
		
		distance = -1.0;
		
		ranging = -1;
		
		response = 0;
		
		link_quality = SUNSET_HDR_CMK_LQ_INIT;
		
		doppler = 0.0;
		
		rssi = 0;
		
		start_tx_time = 0.0;
		
		end_tx_time = 0.0;
		
		start_rx_time = 0.0;
		
		end_rx_time = 0.0;
		
		processing_delay = 0.0;	
		
		phy_src = 0;
		
		phy_dst = 0;
		
		// BACKSEAT STAT
		
		run_id = 0;
		cmd_id = 0;
		
	}
	
	/* Functions to access to the packet fields */
	
	static int offset_;
	
	inline static int& offset() { return offset_; }
	
	inline static Sunset_hdr_common* access(const Packet* p) 
	{
		return (Sunset_hdr_common*) p->access(offset_);
	}
	
} Sunset_hdr_common;


#endif
