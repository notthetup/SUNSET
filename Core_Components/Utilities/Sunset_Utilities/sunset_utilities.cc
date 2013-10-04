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

#include <sunset_utilities.h>

Sunset_Utilities *Sunset_Utilities::instance_ = NULL;

int Sunset_Utilities::experimentMode = 1;	/*!< \brief Default value is set to simulation mode */

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_UtilitiesClass : public TclClass {
	
public:
	Sunset_UtilitiesClass() : TclClass("Sunset_Utilities") {}
	
	TclObject* create(int, const char*const*) {
		
		return (new Sunset_Utilities());
	}
	
} class_Sunset_UtilitiesClass;

Sunset_Utilities::Sunset_Utilities() : TclObject() 
{
	MAX_PKT_SIZE = 0;
	
	// Get variables initialization from the Tcl script
	bind("experimentMode", &experimentMode);
	bind("max_pkt_size_", &MAX_PKT_SIZE);
	instance_ = this;
}

/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script 
 *	@param[in] argc argc is a count of the arguments supplied to the command function.
 *	@param[in] argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_Utilities::command( int argc, const char*const* argv ) 
{
	if ( argc == 2 ) {
		
		/* This start the initialization tasks for the Utilities class */
		
		if (strcmp(argv[1], "start") == 0) {
			
			start();
			return TCL_OK;
		}	
		
		/* This stop the initialization tasks for the Utilities class */
		if (strcmp(argv[1], "stop") == 0) {
			
			stop();
			return TCL_OK;
		}	
	}	
	
	else if ( argc == 3 ) {
		
		/* The "setExperimentMode" value sets the framework to work in emulation or simulation mode. if the value is 1 then the simulation mode will be used, otherwise the emulation mode will be used */
		
		if (strcmp(argv[1], "setExperimentMode") == 0) {
			
			experimentMode = atoi(argv[2]);
			return TCL_OK;
		}	
		
		/* The "setMaxPktSize" value sets the maximal size to be used for the packet in the simulation */
		
		if (strcmp(argv[1], "setMaxPktSize") == 0) {
			
			MAX_PKT_SIZE = atoi(argv[2]);
			return TCL_OK;
		}	
	}	
	
	return TclObject::command(argc, argv);
}


Sunset_Utilities::~Sunset_Utilities()
{
}

/*!
 * 	@brief The eraseData function erases the packet payload. If the framework is running in simulation mode nothing is done. Otherwise, the memory for the packet payload is deallocated.
 * 	@param[in] p The packet we are interested in.
 * 	@param[in] node The node ID that invokes this function.
 */

void Sunset_Utilities::eraseData(Packet* p, int node) 
{
	
	Sunset_Debug::debugInfo(4, node, "Sunset_Utilities::eraseData");
	
	(instance())->removeData(p);
	
	return;
}


/*!
 * 	@brief The erasePkt function erases a packet and its payload. If the framework is running in simulation mode the Packet::free function is called. Otherwise, the memory for the packet payload is first deallocated and the Packet::free function is called.
 * 	@param[in] p The packet to erase.
 * 	@param[in] node The node ID that invokes this function.
 */

void Sunset_Utilities::erasePkt(Packet* p, int node) 
{
	
	Sunset_Debug::debugInfo(4, node, "Sunset_Utilities::erasePkt p %p node %d", p, node);
	
	if (p == 0) {
		
		return;
		
	}
	
	(instance())->removeData(p);
	
	Packet::free(p);
	
	return;
}

/*!
 * 	@brief The copyPkt function copy a packet and its payload. If the framework is running in simulation mode the Packet::copy function is called. Otherwise, the memory for the packet payload is first allocated and then the payload is copied.
 * 	@param[in] p The packet to erase.
 * 	@param[in] node The node ID that invokes this function.
 */

void Sunset_Utilities::copy_data(Packet* p, Packet* copy, int node) 
{
	
	Sunset_Debug::debugInfo(4, node, "Sunset_Utilities::copyPkt node %d p %p", node, p);
	
	if (p == 0) {
		
		return;
		
	}

	(instance())->data_copy(p, copy);
	
	return;
}

/*!
 * 	@brief The eraseOnlyPkt function erases only the packet (calling Packet::free function) and does not erase its payload.
 * 	@param[in] p The packet to erase.
 * 	@param[in] node The node ID that invokes this function.
 */

void Sunset_Utilities::eraseOnlyPkt(Packet* p, int node) 
{	

	Sunset_Debug::debugInfo(4, node, "Sunset_Utilities::eraseOnlyPkt p %p node %d", p, node);

	if (p == 0) {
		
		return;
		
	}
	
	Packet::free(p);
}

/*!
 * 	@brief The compareDouble function compares two double values using a given level of precision.
 * 	@param[in] t1 A double to be compared.
 * 	@param[in] t2 A double to be compared.
 * 	@param[in] sdp The level of accuracy to use for the comparison
 * 	@retval 0 If t1 == t2
 * 	@retval 1 If t1 > t2
 * 	@retval -1 If t1 < t2
 */

int Sunset_Utilities::compareDouble (double t1, double t2, sunset_double_precision sdp) 
{
	double accuracy = 0.0;

	switch (sdp) {

		case SUNSET_DOUBLE_PRECISION_HIGH:

			accuracy = EPSILON_HIGH;

			break;

		case SUNSET_DOUBLE_PRECISION_MEDIUM:

			accuracy = EPSILON_MEDIUM;

			break;

		case SUNSET_DOUBLE_PRECISION_LOW:
			
			accuracy = EPSILON_LOW;

			break;

		default:
			break;
			
	}
	
	if (t1 - accuracy <= t2 && t2 <= t1 + accuracy) {
		
		return 0;
	}
	
	if (t1 < t2) {
		
		return -1;
	}
	
	return 1;
}

/*!
 * 	@brief The getRealTime function returns the current time of the simulation/emulation.
 * 	@retval Current time from the beginning of the simulation/emulation
 */

double Sunset_Utilities::getRealTime() 
{
	if ( isEmulation() ) {
		
		sync_time();
	}
	
	return NOW;
}

/*!
 * 	@brief The scheduleEvent function is used when running in simulation mode and it schedules the event using the standard scheduler (by default the Calendar schedule).
 * 	@param[in] h Event handler.
 * 	@param[in] e Event to be scheduled.
 * 	@param[in] delay Delay to wait before scheduling the event. After delay seconds the event is handled.
 */

void Sunset_Utilities::scheduleEvent(Handler* h, Event* e, double delay) 
{
	Sunset_Debug::debugInfo(4, -1, "Sunset_Utilities::scheduleEvent");
	
	Scheduler& s = Scheduler::instance();	
	
	s.schedule(h, e, delay);	
} 

/*!
 * 	@brief The schedule function schedules the event using the real-time or standard scheduler according to the framework working mode, emulation or simulation mode, respectively.
 * 	@param[in] h Event handler.
 * 	@param[in] e Event to be scheduled.
 * 	@param[in] delay Delay to schedule the event. After delay seconds the event is handled.
 */

void Sunset_Utilities::schedule(Handler* h, Event* e, double delay)
{
	Sunset_Debug::debugInfo(4, -1, "Sunset_Utilities::schedule");
	
	(instance())->scheduleEvent(h, e, delay);	
	
}

/*!
 * 	@brief The setBitByOff function writes "sizevalue" bits of "value" inside inside "val" starting at the "offset" position.
 * 	@param[out] val Destination buffer.
 * 	@param[in] value Input value.
 * 	@param[in] sizevalue Number of bits which have to be read from value and written to val.
 * 	@param[in] offset Offset position to start when writing bits.
 */

void Sunset_Utilities::setBitByOff(unsigned char *val, int32_t value, int sizevalue, int offset) 
{
	
	uint32_t mask = 0;
	uint32_t mask2 = 0;
	int byte = 0;
	
	for(int i = 0, j = 0; i < sizevalue; i++) {
		
		mask = 1 << (i);
		mask2 = 1 << (int)(offset+i)%8;
		
		byte = (int)(floor (((double)offset+i)/8.0));
		
		if(value & mask) {
			
			val[byte] |= mask2;
			
		} else {
			
			val[byte] &= ~mask2;
		} 
	}
	
	return;
}

/*!
 * 	@brief The getBitByOff function reads "sizevalue" bits of from "val" starting from the "offset" position and writes them into "value".
 * 	@param[in] val Buffer to read.
 * 	@param[out] value It will contain the output value.
 * 	@param[in] sizevalue Number of bits which have to be read from val and written to value.
 * 	@param[in] offset Offset position to start when writing bits.
 */

void Sunset_Utilities::getBitByOff(unsigned char *val, int32_t &value, int sizevalue, int offset ) 
{
	
	uint32_t mask = 0;
	uint32_t mask2 = 0;
	int byte = 0;
	
	for ( int c = 0; c < sizevalue; c++ ) {
	 	
	 	mask = 1 << (c);	
		mask2 = 1 << (int)(offset+c)%8;
		
		byte = (int)(floor (((double)offset+c)/8.0));
		
		if(val[byte] & mask2) {
			
			value |= mask;
			
		} else {
			
			value &= ~mask;
		} 		
	}
	
	return;
}


/*!
 * 	@brief The toUpperString converts a given string in its uppercase equivalent.
 * 	@param[in] data String to be converted.
 * 	@param[in] len String length.
 */

void Sunset_Utilities::toUpperString(char* data, int len) 
{
	for(int i = 0; i < len; i++) {
		
		data[i] = toupper(data[i]);
	}
}

/*!
 * 	@brief The toUpperString converts a given string in its uppercase equivalent.
 * 	@param[in] data String to be converted.
 */

void Sunset_Utilities::toUpperString(string data) 
{
	for(int i = 0; i < strlen(data.c_str()); i++) {
		
		data[i] = toupper(data[i]);
	}
}


/*!
 * 	@brief The setBitsByByte function writes "sizevalue" bits of byte "value" inside the "val" buffer, starting from the "offset" position. SizeValue is always <= 8
 */

void Sunset_Utilities::setBitsByByte(char *val, uint8_t value, int sizevalue, int offset) 
{
	uint8_t mask = 0;
	uint8_t mask2 = 0;
	int byte = 0;
	
	for(int i = 0, j = 0; i < sizevalue; i++) {
		
		mask = 1 << (i);
		mask2 = 1 << (int)(offset+i)%8;
		
		byte = (int)(floor (((double)offset+i)/8.0));
		
		if(value & mask) {
			
			val[byte] |= mask2;
			
		} else {
			
			val[byte] &= ~mask2;
		} 
	}
	
	return;
}

/*!
 * 	@brief ThgetBitsByByte function reads "sizevalue" bits of "val" starting from the "offset" position and stores them into the byte "value". SizeValue is always <= 8
 */

void Sunset_Utilities::getBitsByByte(char *val, uint8_t &value, int sizevalue, int offset ) 
{
	uint8_t mask = 0;
	uint8_t mask2 = 0;
	int byte = 0;
	
	for ( int c = 0; c < sizevalue; c++ ) {
	 	
	 	mask = 1 << (c);	
		mask2 = 1 << (int)(offset+c)%8;
		
		byte = (int)(floor (((double)offset+c)/8.0));
		
		if(val[byte] & mask2) {
			
			value |= mask;
			
		} else {
			
			value &= ~mask;
		} 
		
	}
	
	return;
}

/*!
 * 	@brief The setBits function writes "sizevalue" bits of "buffer" inside "val" starting from the "offset" position. Users have to avoid the truncation of useful information. This can happen if sizeval is lower than the number of bits needed to correctly represent buffer, i.e. when negative numbers are used and the sign bit is not stored. 
 */

void Sunset_Utilities::setBits(char *val, char* buffer, int sizevalue, int offset) 
{
	int numBytes = (int) (floor(sizevalue / 8));
	int aux = 0;
	
	for (int i = 0; i < numBytes; i++) {
		
		setBitsByByte(val, (uint8_t)(buffer[i]), 8, offset + (i * 8));
	}
	
	aux = sizevalue - (numBytes * 8);
	
	setBitsByByte(val, (uint8_t)(buffer[numBytes]), aux, offset + (numBytes * 8));
	
	return;
}

/*!
 * 	@brief The getBits function reads "sizevalue" bits of "val" starting from the "offset" position and stores them into buffer.
 */

void Sunset_Utilities::getBits(char *val, char* buffer, int sizevalue, int offset) 
{
	int numBytes = (int)(floor(sizevalue / 8));
	int aux = 0;
	
	for (int i = 0; i < numBytes; i++) {
		
		getBitsByByte(val, (uint8_t&)(buffer[i]), 8, offset + (i * 8));
	}
	
	aux = sizevalue - (numBytes * 8);
	
	getBitsByByte(val, (uint8_t&)(buffer[numBytes]), aux, offset + (numBytes * 8));
	
	return;
}

/*!
 * 	@brief  The setPriority function sets the priority of the current process to "prio".
 * 	@param prio The priority to be set.
 * 	@retval res The setpriority result.
 */

int Sunset_Utilities::setPriority(int prio) 
{
	int res;
	
	res = setpriority(PRIO_USER, 0, prio);
	
	if ( res != 0 ) {
		
		switch(errno)  {
				
			case EACCES:
				Sunset_Debug::debugInfo(-1, -1, "Sunset_Utilities::setPriority Permission Denied! - Check the root permission");
				
				break;
				
			default:
				perror("Sunset_Utilities::setPriority");
				
				break;
		}	
	}
	
	return res;
}

/*!
 * 	@brief  The getPriority function gets the priority of the current process.
 * 	@retval prio The priority of the current process.
 */

int Sunset_Utilities::getPriority() 
{
	int prio;
	
	errno = 0;
	
	prio =  getpriority(PRIO_PROCESS, 0);
	
	if ( errno == -1 ) {
		
		perror("Sunset_Utilities::getPriority");				
	}
	
	return prio;
}

/*!  @brief The getNOW function returns the time in seconds from the beginning of the simulation. 
 *    @return The current time in seconds. 
 */

double Sunset_Utilities::getNOW()
{
	return Scheduler::instance().clock();
}

/*!  @brief The getEpoch function returns the emulation time in seconds reflecting the global timing (epoch). 
 *    @return The epoch time in seconds. 
 */

double Sunset_Utilities::getEpoch()
{
	return NOW;
}

/*!
 * 	@brief The get_now function returns the simulation/emulation time from the beginning of the experiment. 
 */

double Sunset_Utilities::get_now()
{
	return(instance())->getNOW();
}

/*!
 * 	@brief The get_epoch function returns the absolute simulation/emulation time according to the framework working mode, emulation or simulation mode, respectively. 
 */

double Sunset_Utilities::get_epoch()
{
	return (instance())->getEpoch();	
}

/*!
 * 	@brief The get_pkt_size function returns the final packet size of the packet in input for in water transmission. When running in simulation mode it is the value defined in the header common, when running in emulation it considers the conversion overhead.
 * 	@param  p The packet to be transmitted.
 * 	@return The final packet size in bytes.
 */

int Sunset_Utilities::get_pkt_size(Packet* p)
{
	
	return (instance())->my_pkt_size(p);	
}

/*!
 * 	@brief The get_pkt_size function computes the overhead to be added to a size amount of information for in water transmission. When running in simulation mode it is 0, when running in emulation it is related to the conversion overhead.
 * 	@param  size The amount of information to be transmitted.
 * 	@return The final amount of bytes.
 */

int Sunset_Utilities::get_pkt_size(int size)
{
	
	return (instance())->my_pkt_size(size);	
}

/*!
 * 	@brief The get_max_pkt_size function returns the maximal allowed packet size.
 * 	@return The maximal amount of bytes for a packet.
 */

int Sunset_Utilities::get_max_pkt_size()
{
	
	return (instance())->my_max_pkt_size();	
}

/*!
 * 	@brief The my_pkt_size function returns the final packet size of the packet in input for in water transmission. When running in simulation mode it is the value defined in the header common.
 * 	@param  p The packet to be transmitted.
 * 	@return The final packet size in bytes.
 */

int Sunset_Utilities::my_pkt_size(Packet* p)
{
	
	return HDR_CMN(p)->size();	
}

/*!
 * 	@brief The my_pkt_size function computes the overhead to be added to a size amount of information for in water transmission. When running in simulation mode it is 0.
 * 	@param  size The amount of information to be transmitted.
 * 	@return The final amount of bytes.
 */

int Sunset_Utilities::my_pkt_size (int size)
{
	
	return size;	
}

/*!
 * 	@brief The my_max_pkt_size function returns the maximal allowed packet size when running in simulation mode. It is defined by the values in MAX_PKT_SIZE.
 * 	@return The maximal amount of bytes for a packet.
 */

int Sunset_Utilities::my_max_pkt_size()
{
	
	Sunset_Debug::debugInfo(4, -1, "Sunset_Utilities::my_max_pkt_size val %d", MAX_PKT_SIZE);

	return MAX_PKT_SIZE;  
}

/*!
 * 	@brief The start function performs the initialization tasks of the Utilities class. When running in simulation mode no intializations are needed.
 */

void Sunset_Utilities::start()
{ 	
	if ( instance_ == NULL ) {
	
		Sunset_Debug::debugInfo(-1, -1, "azzo!\n");
		exit(0);
	}
	
	return; 
}

/*!
 * 	@brief The stop function stops the tasks of the Utilities class. 
 */

void Sunset_Utilities::stop()
{ 	
	return;    
}
