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



#ifndef SUNSET_AGENT_PKT_H
#define SUNSET_AGENT_PKT_H

#include <packet.h>
#include <stdint.h>
#include <vector>

#define MAXDATASIZE		256

#define	SUNSET_AGT_ProtocolVersion	0x00

#define SUNSET_AGT_Type_Management	0x00
#define SUNSET_AGT_Type_Control	0x01
#define SUNSET_AGT_Type_Data		0x02
#define SUNSET_AGT_Type_Reserved	0x07

#define SUNSET_AGT_Subtype_Data	0x00
#define SUNSET_AGT_Subtype_Ctrl	0x01
#define SUNSET_AGT_Subtype_Request	0x02
#define SUNSET_AGT_Subtype_Response	0x03
#define SUNSET_AGT_Subtype_GPS		0x04
#define SUNSET_AGT_Subtype_GPS2	0x05

#define HDR_SUNSET_AGT(P) (hdr_Sunset_Agent::access(P))

extern packet_t PT_SUNSET_AGT;

/*! @brief The control frame of the Agent packet. */

struct sunset_agt_control 
{
	u_char		ac_type			: 3; /*!< \brief The type of the Agent packet. */
	u_char		ac_subtype		: 3; /*!< \brief The subtype of the Agent packet. */
	u_char		ac_protocol_version	: 2; /*!< \brief The version of the Agent packet. */
};

/*! @brief The Agent packet header */

typedef struct hdr_Sunset_Agent 
{                                                                                                                 
	struct sunset_agt_control ac;
	
	int pkt_id;
	int src_id;
	int dst_id;
	char* data; /* This is the packet payload */
	int data_size;
		
	/* Functions to access to the packet fields */
	
	inline int& pktId() {return (pkt_id);}
	inline int& dataSize() {return (data_size);}
	inline int& srcId() {return (src_id);}
	inline int& dstId() {return (dst_id);}
	inline char* getData() { return data; }
	
	static int offset_;
	inline static int& offset() { return offset_; }
	
	inline static hdr_Sunset_Agent* access(const Packet* p) {
		return (hdr_Sunset_Agent*) p->access(offset_);
	}
	
} hdr_Sunset_Agent;

#endif
