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

#ifndef __Sunset_Mac_Pkt_h__
#define __Sunset_Mac_Pkt_h__

#include <packet.h>
#include <mac.h>
#define	SUNSET_MAC_ProtocolVersion	0x00

#define SUNSET_MAC_Type_Management	0x00

#define SUNSET_MAC_Type_Control		0x01
#define SUNSET_MAC_Type_Data		0x02
#define SUNSET_MAC_Type_Reserved	0x03

#define SUNSET_MAC_Subtype_RTS		0x0B
#define SUNSET_MAC_Subtype_CTS		0x0C
#define SUNSET_MAC_Subtype_ACK		0x0D
#define SUNSET_MAC_Subtype_Data		0x04
#define SUNSET_MAC_Subtype_WARN		0x0A
#define SUNSET_MAC_Subtype_TONE		0x0E

#define HDR_SUNSET_MAC(P) (hdr_Sunset_Mac::access(P))

#define MAX_FRAG_NUM 	100
#define FRAG_HDR_SIZE 	3

extern packet_t PT_SUNSET_MAC;

/*! @brief The control frame of the MAC packet. */

struct sunset_mac_frame_control 
{
	u_char		fc_type			: 2; /*!< \brief The type of the MAC packet. */
	u_char		fc_subtype		: 4; /*!< \brief The subtype of the MAC packet. */
	u_char		fc_protocol_version	: 2; /*!< \brief The version of the MAC packet. */
	
	u_char		fc_order		: 1;
	u_char		fc_wep			: 1;
	u_char		fc_more_data		: 1;
	u_char		fc_pwr_mgt		: 1;
	u_char		fc_retry		: 1;
	u_char		fc_more_frag		: 1;
	u_char		fc_from_ds		: 1;
	u_char		fc_to_ds		: 1;
};

/*! @brief The MAC packet header */

typedef struct hdr_Sunset_Mac 
{
	
	struct sunset_mac_frame_control	dh_fc;
	
	u_int16_t		dh_duration;
	int				src;
	int				dst;
	int				source;
	u_int16_t		pktId;
	u_int16_t		macSize;
	
	/* information used in case of MAC protocol supporting data packet fragmentation */
	
	int				frag_a_index;
	int				frag_r_index;
	int				frag_a_total;
	int				frag_r_total;
	int				fragAck[MAX_FRAG_NUM];
	
	/* Functions to access to the packet fields */
	
	inline int& macSrc() { return src; }
	inline int& macDst() { return dst; }
	
	static int      offset_;
	inline static int& offset() { return offset_; }
	
	inline static hdr_Sunset_Mac* access(const Packet* p) 
	{
		return (hdr_Sunset_Mac*) p->access(offset_);
	}
	
} sunset_mac;

/*! @brief The RTS header of the MAC packet. */

typedef struct sunset_rts_frame 
{
	struct sunset_mac_frame_control	rf_fc;
	
	u_int16_t		rf_duration;
	int		src;
	int		dst;
	int		source;
	u_int16_t		pktId;
	
} sunset_rts_frame;

/*! @brief The CTS header of the MAC packet. */

typedef struct sunset_cts_frame 
{
	struct sunset_mac_frame_control	cf_fc;
	
	u_int16_t		cf_duration;
	int		src;
	int		dst;
	int		source;
	u_int16_t		pktId;
	
} sunset_cts_frame;

/*! @brief The ACK header of the MAC packet. */

typedef struct sunset_ack_frame 
{
	struct sunset_mac_frame_control	af_fc;
	
	u_int16_t		af_duration;
	int		src;
	int		dst;
	int		source;
	u_int16_t		pktId;
	int fragAck[MAX_FRAG_NUM];
	
} sunset_ack_frame;


#endif
