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

#ifndef __Sunset_PktConverter_h__
#define __Sunset_PktConverter_h__

#include <stdlib.h>
#include <map>
#include <set>
#include <list>
#include <stdint.h>

#include "config.h"
#include "packet.h"

#include <math.h>
#include <sunset_debug.h>
#include <sunset_utilities.h>
#include <bitset>
#include <iostream>

#define TIME_PRECISION 		1.0e2
#define MAX_BITSET_SIZE 	256

/*!
 @brief This data structure represents the different header types currently extending the general packet converter class. 
 Additional packet header converters should be added here if they implements virtual methods such as buffer2Pkt(), pkt2Buffer(), etc.  */

typedef enum {
	
	UW_PKT_NS = 1,
	UW_PKT_PHY = 2,
	UW_PKT_MAC = 3,
	UW_PKT_ROUTING = 4,
	UW_PKT_AGENT = 5,
	UW_PKT_CBR = 6,
	UW_PKT_NONE = 7
	
} sunset_header_type;

/*! @brief The generic packet converter class handles all the information about ns-2 packets when running in emulation mode. It is responsable for the conversion of the different ns-2 packet headers into a stream of bytes for the external devices and vice versa. All the different considered packet types, for the different protocol stack layers, extend this class and implement their own packet to bytes and bytes to packet conversion methods. */

class Sunset_PktConverter : public TclObject {
public:
	
	Sunset_PktConverter();
	~Sunset_PktConverter() { instance_ = NULL; }
	
	virtual int command( int argc, const char*const* argv );
	
	/*! @brief Function to convert a packet p in a stream of bytes. */
	
	char* pkt2Buffer(Packet* p, int& length);
	
	/*! @brief Function to convert a stream of bytes in a packet p. */
	
	int buffer2Pkt(Packet* p, char* buffer, int& length);
	
	/*! @brief Function to convert a stream of bytes in a packet p when source and destination IDs are provided by the modem. */
	
	int buffer2Pkt(int src, int dst, Packet* p, char* buffer, int& length);
	
	/*! @brief Function to delete memory allocated for packet p. */
	
	void erasePkt(Packet* p);
	
	/*! @brief Function returning the size of the stream of bytes for the packet p conversion. */
	
	virtual int getConvertedInfoLength(Packet* p);
	
	/*! @brief General access to pkt_converter object. */
	
	static Sunset_PktConverter* instance() 
	{
		if (instance_ == NULL) {
			
			Sunset_PktConverter();
		}
		
		return (instance_);
	}
	
	int getMaxDataSize() { return MAX_DATA_SIZE; } /*!< \brief It returns the maximum packet size in bytes decided by the user in the tcl script. */
	
	int setMaxDataSize(int val) { MAX_DATA_SIZE = val; } 
	
	int getMaxPayloadSize();
	
	Packet* createMiniPkt(sunset_header_type m, int src, int dst, int pkt_sub_type); /*!< \brief Method to create a mini packet, according to the different registered packet header converters. (Currently used for WHOI MicroModem)*/
	
	void getSrc(Packet* p, sunset_header_type m, int& src); // return the src ID of the header type m in packet p
	
	void getDst(Packet* p, sunset_header_type m, int& dst); // return the dst ID of the header type m in packet p
	
	void getPktSubType(Packet* p, sunset_header_type m, int& subType); // return the pkt subtype of the header type m in packet p
	
	void getPktType(Packet* p, sunset_header_type m, int& type); // return the pkt type of the header type m in packet p
	
	int isDataPkt(Packet* p, sunset_header_type m); // return 1 if p is a data packet according to the header type m in packet p 
	
	int isCtrlPkt(Packet* p, sunset_header_type m); // return 1 if p is a control packet according to the header type m in packet p 
	
	int isApplicationPkt(int pktType); // return 1 if the input packet type corresponds to an application packet

	int useMiniPkt(Packet* p, sunset_header_type m);
	
	int getAddrBits() { return ADDR_BITS; } /*!< \brief It returns the maximum number of bits that have to be used when converting node IDs inside the packet headers. */
	
	int getPktIdBits() { return PKT_ID_BITS; } /*!< \brief It returns the maximum number of bits that have to be used when converting packet IDs inside the packet headers. */
	
	int getTimeBits() { return TIME_BITS; } /*!< \brief It returns the maximum number of bits that have to be used when converting timing information inside the packet headers. */
	
	int getLevelIdBits() { return LEVEL_ID_BITS; } /*!< \brief It returns the maximum number of bits that have to be used to represent the number of packet header converter modules attached to the main packet converter class. */
	
	int getDataBits() { return DATA_BITS; } /*!< \brief It returns the maximum number of bits used to express the length of the payload (in bytes) when converting it into a stream of bytes. */

	void data_copy(Packet *p, Packet* copy); /*!< \brief It copies packet p header and payload. */
	
	int getHeaderSizeBytes(); /*!< \brief It returns the number of bytes needed when converting all the considered ns-2 packet header. */
	
	int getPktTypeBits() { return PKT_TYPE_BITS; } /*!< \brief It returns the maximum number of bits that have to be used to represent the ns2 pkt type used. */
	
protected:
	
	virtual int getHeaderSizeBits(); /*!< \brief It returns the number of bits needed when converting all ns-2 packet header. */
	
	virtual int pkt2Buffer(int level, Packet* p, char* buffer, int length) {} /*!< \brief  Method to to convert a ns-2 packet in a stream of bytes, extended by each packet header converter module. */
	
	virtual int buffer2Pkt(int level, Packet* p, char* buffer, int length, int bits) {} /*!< \brief  Method to to convert a stream of bytes into a ns-2 packet, extended by each packet header converter module. */
	
	virtual int buffer2Pkt(int level, int src, int dst, Packet* p, char* buffer, int length, int bits) {} /*!< \brief  Method to to convert a stream of bytes with specific source and dest IDs into a ns-2 packet, extended by each packet header converter module. */
	
	virtual void erasePkt(int level, Packet* p) { return; } /*!< \brief Method to erase packet information, extended by each packet header converter module. */
	
	virtual int createMiniPkt(int level, Packet* p, sunset_header_type m, int src, int dst, int pkt_sub_type) { return 1; } /*!< \brief Method to create a mini packet, extended by each packet header converter module. (Currently used for WHOI MicroModem)*/
	
	virtual int getSrc(int level, Packet* p, sunset_header_type m, int& src) { return 1; } /*!< \brief Method to obtain the source ID for a specific packet header, extended by each packet header converter module. */
	
	virtual int getDst(int level, Packet* p, sunset_header_type m, int& dst) { return 1; } /*!< \brief Method to obtain the destination ID for a specific packet header, extended by each packet header converter module. */
	
	virtual int getPktSubType(int level, Packet* p, sunset_header_type m, int& subType) { return 1; } /*!< \brief Method to obtain the packet subtype for a specific packet header, extended by each packet header converter module. */
	
	virtual int getPktType(int level, Packet* p, sunset_header_type m, int& type) { return 1; } /*!< \brief Method to obtain the packet typw for a specific packet header, extended by each packet header converter module. */
	
	virtual int isDataPkt(int level, Packet* p, sunset_header_type m, int& result) { return 1; } /*!< \brief Method to check if the packet is a data packet for a specific packet header, extended by each packet header converter module. */
	
	virtual int isCtrlPkt(int level, Packet* p, sunset_header_type m, int& result)  { return 1; } /*!< \brief Method to check if the packet is a control packet for a specific packet header, extended by each packet header converter module. */
	
	virtual int isApplicationPkt(int level, int pktType, int& result) { return 1; } /*!< \brief Method to check if the packet type in input corresponds to an application packet, it can be extended by each packet header converter module. */

	virtual int useMiniPkt(int level, Packet* p, sunset_header_type m, int& result)  { return 1; } /*!< \brief Method to check if the packet can be converted as a mini pkt for a specific packet header, extended by each packet header converter module. */
	
	/*! @brief The getName prints the packet converter module name. */
	virtual void getName() { Sunset_Debug::debugInfo(5, -1, "Sunset_PktConverter"); }
	
	void setBits (char *val, char* buffer, int sizevalue, int offset); // write sizevalue bits of information from buffer+offset to val
	
	void getBits (char *val, char* buffer, int sizevalue, int offset); // write sizevalue bits of information from val+offset to buffer
	
	/*! @brief The sec function converts a timestamp 't' expressed using an integer value into a double integer value with the original decimal precision. */
	inline static double sec(double t) { return(t /= TIME_PRECISION); }
	
	/*! @brief The usec function converts a timestamp 't' expressed using double integer into an integer value considering TIME_PRECISION decimal digits.*/
	inline static u_int32_t usec(double t){
		
		u_int32_t us = (u_int32_t)floor((t *= TIME_PRECISION) + 0.5);
		return us;
	}
	
	/*! @brief The sec function converts a timestamp 't' expressed using an integer value into a double integer value with a given decimal precision. */
	inline static double sec(double t, int precision) { return(t /= precision); }
	
	/*! @brief The usec function converts a timestamp 't' expressed using double integer into an integer value  with a given decimal precision.*/
	inline static u_int32_t usec(double t, int precision){
		
		u_int32_t us = (u_int32_t)floor((t *= precision) + 0.5);
		return us;
	}
	
	/*! @brief The assignNsPktType function store the information that he packet type in input is used and assign an ID to this packet to be used during the conversion operation. */
	static void assignNsPktType(int type);
	
	/*! @brief The getNsPktType function return the ID assigned to the packet type in input. */
	static int getNs2PktType(int type);
	
	/*! @brief The getPkt2NsType function return the ns2 packet type corresponding to the ID in input. */
	static int getPkt2NsType(int id);
	
	virtual int checkPktLevel(Packet* p); // return 1 if the specific packet header converter is interested in the input packet, 0 otherwise 
	
	virtual int data_copy(int level, Packet *p, Packet* copy) { return 0; }
	
	virtual void start(); //initialize module variable and interaction with othe modules
	
	virtual void stop();	//clean module initialization
	
private:
	
	set<int> getPktLevels(Packet* p); // return the set of id for the different packet header converters which  are involved in the operation on the input packet 
	
private:
	
	int ADDR_BITS;      		/*!< \brief number of bits used for the node IDs */
	
	int MAX_DATA_SIZE;  		/*!< \brief maximum length for a data packet in bytes */
	
	int DATA_BITS;     		/*!< \brief number of bits used to store the data packet length */
	
	int PKT_ID_BITS;    		/*!< \brief number of bits used for pkt ID */
	
	int TIME_BITS;      		/*!< \brief number of bits used for timing information */

	static int PKT_TYPE_BITS;      		/*!< \brief number of bits used for ns packet type information */
	
	static int LEVEL_ID_BITS;   	/*!< \brief number of bits used to express the ID of the packet converter module, it depends on maxLevelId */
	
	int maxLevelId;             	/*!< \brief maximum number of packet converter modules */
	
	map<int, Sunset_PktConverter*> pktConverterInfo; /*!< \brief it contains the reference to the differnt packet converters registered in the tcl script */
	
	static Sunset_PktConverter* instance_;
	
	static int pkt_type_counter; // used to count the number of pkt type used
	
	static map<int, int> pkt2id; 	// store the association between ns pkt type and converter pkt type
	
	static map<int, int> id2pkt; 	// store the association between ns pkt type and converter pkt type
	
};

#endif
