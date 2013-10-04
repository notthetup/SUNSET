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



#ifndef __Sunset_Routing_Pkt_h__
#define __Sunset_Routing_Pkt_h__

#include <packet.h>
#include <stdint.h>

#define	SUNSET_RTG_ProtocolVersion	0x00

#define SUNSET_RTG_Type_Management	0x00
#define SUNSET_RTG_Type_Control	0x01
#define SUNSET_RTG_Type_Data		0x02
#define SUNSET_RTG_Type_Reserved	0x03

#define SUNSET_RTG_Subtype_Data	0x0A
#define SUNSET_RTG_Subtype_BEACON	0x0B
#define SUNSET_RTG_Subtype_ADV		0x0C
#define SUNSET_RTG_Subtype_3		0x0D

#define HDR_SUNSET_RTG(P) (hdr_Sunset_Routing::access(P))

extern packet_t PT_SUNSET_RTG;

/*! @brief The control frame of the Routing packet. */
struct sunset_routing_control 
{
	u_char		rc_type			: 2; /*!< \brief The type of the Routing packet. */
	u_char		rc_subtype		: 4; /*!< \brief The subtype of the Routing packet. */
	u_char		rc_protocol_version	: 2; /*!< \brief The version of the Routing packet. */
};

/*! @brief The Routing packet header */

typedef struct hdr_Sunset_Routing 
{
	
	struct sunset_routing_control rc;
	
	/* Functions to access to the packet fields */
	
	uint8_t* data;
	u_int16_t size_;
	inline u_int16_t& size() {return (size_);}
	inline uint8_t* getData() { return data; }
	static int      offset_;
	inline static int& offset() { return offset_; }
	
	inline static hdr_Sunset_Routing* access(const Packet* p) {
		
		return (hdr_Sunset_Routing*) p->access(offset_);
	}
	
}hdr_Sunset_Routing;

#endif
