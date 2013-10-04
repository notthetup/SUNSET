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



#ifndef __Sunset_Utilities_h__
#define __Sunset_Utilities_h__

#include <map>
#include <set>
#include <list>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>
#include <pthread.h>

#include "packet.h"
#include <sunset_debug.h>

#define EPSILON_HIGH 		0.00001
#define EPSILON_MEDIUM 		1e-3
#define EPSILON_LOW 		1e-2

#define EPSILON 		0.00001
#define SIMULATION_MODE 	1
#define EMULATION_MODE 		0

#define NEGATIVE_SIGN		1
#define POSITIVE_SIGN		0


#define BAUD_CONV 		10
#define SOUND_SPEED_IN_WATER	1500.0		//1500m/s

#ifdef NOW
	#undef NOW
#endif

#define NOW Sunset_Utilities::get_now()

typedef nsaddr_t NodeAddress; 				/*!< \brief Node ID */

typedef std::set<NodeAddress> NodeList; 		/*!< \brief Nodes list */

typedef std::map<NodeAddress, double> NodeListDistance; /*!< \brief A list of distances among nodes */

typedef enum {	//Double precisions used to compare double values
	
	SUNSET_DOUBLE_PRECISION_HIGH = 1,
	SUNSET_DOUBLE_PRECISION_MEDIUM = 2,
	SUNSET_DOUBLE_PRECISION_LOW = 3
	
} sunset_double_precision;

/*! @brief This class is used to define utility functions used when running in simulation/emulation mode. */

class Sunset_Utilities: public TclObject {
	
public:
	
	Sunset_Utilities();
	~Sunset_Utilities();
	
	virtual int command( int argc, const char*const* argv );
	
	/*!
	 * 	@brief The compareDouble function compares two double values using a given level of precision.
	 */
	static int compareDouble (double t1, double t2, sunset_double_precision sdp);
		
	/*!
	 * 	@brief The getRealTime function returns the current time of the simulation/emulation.
	 */
	static double getRealTime();
	
	/*!
	 * 	@brief This function updates the simulation/emulation time synchronizing the time at the scheduler.
	 */
	inline static void sync_time() 
	{ 	
		Scheduler& s = Scheduler::instance();
		s.sync(); 
	}
	
	/*!
	 * 	@brief The eraseData function erases the payload of packet p.
	 */
	void static eraseData(Packet* p, int node = -1);
	
	/*!
	 * 	@brief The eraseData function erases a packet p and its payload.
	 */
	void static erasePkt(Packet* p, int node = -1);
	
	/*!
	 * 	@brief The eraseData function erases a packet p but it does not erase its payload.
	 */
	void static eraseOnlyPkt(Packet* p, int node = -1);
	
	/*!
	 * 	@brief The copyPkt function copies a packet and its payload.
	 */
	void static copy_data(Packet* p, Packet* copy, int node = -1);
	
	/*!
	 * 	@brief This function returns the final packet size of the packet in input for in water transmission. When running in simulation mode it is the value defined in the header common, when running in emulation it considers the conversion overhead.
	 */
 	int static get_pkt_size (Packet* p);
	
	/*!
	 * 	@brief This function computes and adds the overhead to be added to size of information for in water transmission. When running in simulation mode it is 0, when running in emulation it is related to the conversion overhead.
	 */
 	int static get_pkt_size (int size);
	
	/*!
	 * 	@brief This function returns the maximal allowed packet size.
	 */
 	int static get_max_pkt_size ();
	
	/*!
	 * 	@brief This function returns 1 if the framework is working in simulation mode, 0 otherwise.
	 */
	int static isSimulation() { return experimentMode == SIMULATION_MODE; }
	
	/*!
	 * 	@brief This function returns 1 if the framework is working in emulation mode, 0 otherwise.
	 */
	int static isEmulation() { return experimentMode == EMULATION_MODE; }
	
	/*!
	 * 	@brief The schedule function schedules the event using the real-time or standard scheduler according to the framework working mode, emulation or simulation mode, respectively.
	 */
	static void schedule(Handler*, Event*, double delay);
	
	/*!
	 * 	@brief The setBitByOff function writes "sizevalue" bits of "value" inside inside "val" starting from at the "offset" position.
	 */
	static void setBitByOff(unsigned char *, int32_t, int, int);
	
	/*!
	 * 	@brief The getBitByOff function reads "sizevalue" bits of from "val" starting from the "offset" position and writes them into "value".
	 */
	static void getBitByOff(unsigned char *, int32_t &, int, int);
	
	/*!
	 * 	@brief This writes "sizevalue" bits of "buffer" inside "val" starting at the "offset" position. Users have to avoid the truncation of useful information. This can happen if sizeval is lower than the number of bits needed to correctly represent buffer, i.e. when negative numbers are used and the sign bit is not stored. 
	 */
	static void setBits (char *val, char* buffer, int sizevalue, int offset);
	
	/*!
	 * 	@brief This reads "sizevalue" bits of "val" starting from the "offset" position and stores them into buffer.
	 */
	static void getBits (char *val, char* buffer, int sizevalue, int offset);
	
	/*!
	 * 	@brief This function returns the instance to the utilities class.
	 */
	static Sunset_Utilities* instance() 
	{
		
		if (instance_ == NULL) {
			Sunset_Utilities();
		}
		
		return (instance_);		/*!< General access to utilities object */
	}
	
	/*!
	 * 	@brief The toUpperString converts a given string in its uppercase equivalent.
	 */
	static void toUpperString(char* data, int len);
	static void toUpperString(string data);
	
	/*!
	 * 	@brief  This function sets the priority of the current process to "prio".
	 */
	static int setPriority(int );
	static int getPriority();
	
	/*!
	 * 	@brief The get_now function returns the simulation/emulation time from the beginning of the experiment. 
	 */
	static double get_now();
	
	/*!
	 * 	@brief The get_epoch function returns the absolute simulation/emulation time according to the framework working mode, emulation or simulation mode, respectively. 
	 */
	static double get_epoch();
	
	
protected:
	
	/*!
	 * 	@brief This function erases the payload of the specified packet, when running in simulation mode no actual payload should be created and nothing should be erased.
	 */
	virtual void removeData(Packet* p) {} 
	
	virtual void data_copy(Packet* p, Packet* copy) { } 
	virtual void scheduleEvent(Handler*, Event*, double delay);
	
	virtual double getNOW();
	virtual double getEpoch();
	
 	virtual int my_pkt_size(Packet* p);
	
 	virtual int my_pkt_size(int size);
	
 	virtual int my_max_pkt_size();
	
 	virtual void start();
 	virtual void stop();
	
	static Sunset_Utilities* instance_;
	
private:
	
  	static void setBitsByByte(char *val, uint8_t value, int sizevalue, int offset);
	static void getBitsByByte(char *val, uint8_t &value, int sizevalue, int offset);
	
	/*! @brief The sec() function converts a timestamp "t" expressed using an integer value into a double integer value. */
	inline static double sec(double t) { return (t *= 1.0e-3); }
	
	/*! @brief The usec function converts a timestamp "t" expressed using double integer into an integer value. */
	inline static u_int32_t usec(double t) 
	{
		u_int32_t us = (u_int32_t)floor((t *= 1e3) + 0.5);
		
		return us;
	}
	
	static int experimentMode;	/*!< \brief 1 = simulation - 0 = emulation with real hardware */
	
	int MAX_PKT_SIZE;
	
};

extern int errno;

#endif
