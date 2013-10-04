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

#ifndef __Sunset_Timing_h__
#define __Sunset_Timing_h__

#include <termios.h>
#include <packet.h>
#include <sunset_debug.h>
#include <sunset_utilities.h>

/*!
 @brief We currently assume the possibility of two different bit rates, one for data and one for control information.
 The user can set, using the TCP script, the same or different values for such bit rates when running in simulation mode. 
 When running in emulation mode the specific bit rate depends on the device used for the transmissions. */

typedef enum {
	
	TIMING_DATA_RATE = 1,
	TIMING_CTRL_RATE = 2	
	
} sunset_rateType;


/*! @brief This class is used to compute the correct timing delays when running in simulation and emulation mode. 
 Delays related to the use of embedded device, acoustic modem or other devices can be added to this class to compute timing and delays closer to the one would be obtained in real-life testing. 
 The timing information will be used by the different layers of the protocol stack when they have to compute timeouts or other time related information. */

class Sunset_Timing: public TclObject {
	
public:
	
	Sunset_Timing();
	~Sunset_Timing() { }
	
	virtual int command( int argc, const char*const* argv );
	
	//size in assumed to be in Bytes
	virtual double txtime(int size, sunset_rateType rate = TIMING_DATA_RATE);	
	
	//dataSize in assumed to be in Bytes
	virtual int getPktSize (int dataSize);
	
	//dataSize in assumed to be in Bytes
	virtual int getPktSize (Packet* p);
	
	//size is assumed to be in Bytes
	double txtime(int size, double rate);
	
	//size is assumed to be in Bytes
	double transfertTime(int size);	//transfert time of information from driver to modem on serial connection
	
	/*!
	 * 	@brief The getBaudRate function returns the baudrate (expressed as an integer value). It is specified by the user in the TCL script.
	 */
	int getBaudRate() { return baudRate; }
	
	
	/*!
	 * 	@brief The getDeviceDelay function returns the computation delay when running on embedded device. It is specified by the user in the TCL script.
	 */
	double getDeviceDelay() { return deviceComputationaldelay; }
	
	/*!
	 * 	@brief The getDeviceDelayCtrl function returns the computation delay related to control information when running on embedded device. It is specified by the user in the TCL script.
	 */
	double getDeviceDelayCtrl() { return deviceComputationalDelayCtrl; }
	
	/*!
	 * 	@brief The getDeviceDelayData function returns the computation delay related to data information when running on embedded device. It is specified by the user in the TCL script.
	 */
	double getDeviceDelayData() { return deviceComputationalDelayData; }
	
	/*!
	 * 	@brief The getDataRate function returns the bit rate to transmit data packet. It is specified by the user in the TCL script.
	 */
	double getDataRate() { return dataRate; }
	
	/*!
	 * 	@brief The getCtrlRate function returns the bit rate to transmit control packet. It is specified by the user in the TCL script.
	 */
	double getCtrlRate() { return ctrlRate; }
	
	/*!
	 * 	@brief The getModemDelay function returns the delay related to acoustic modem operations. It is specified by the user in the TCL script.
	 */
	double getModemDelay() { return modemProcessingdelay; }
	
	/*!
	 * 	@brief The getModemDelayCtrl function returns the delay related to acoustic modem operations for control packets. It is specified by the user in the TCL script.
	 */
	double getModemDelayCtrl() { return modemProcessingDelayCtrl; }
	
	/*!
	 * 	@brief The getModemDelayData function returns the delay related to acoustic modem operations for data packets. It is specified by the user in the TCL script.
	 */
	double getModemDelayData() { return modemProcessingDelayData; }
	
	/*!
	 * 	@brief The getMaxPropagationDelay function returns the maximum propagation delay assumed in the network. It is specified by the user in the TCL script.
	 */
	double getMaxPropagationDelay() { return maxPropagationDelay; }
	
	/*!
	 * 	@brief The getSIFS function returns the Short InterFrame Space (SIFS). It is specified by the user in the TCL script.
	 */
	double getSIFS() { return SIFS; }
	
	/*!
	 * 	@brief The getSoundSpeedInWater function returns assumed speed of sound in water. It is specified by the user in the TCL script.
	 */
	double getSoundSpeedInWater() { return SOUND_SPEED_IN_WATER; }
	
	/*!
	 * 	@brief The overheadTime function returns the additional delay when transmitting "size" bytes of data. 
	 */
	double overheadTime(int size);
	
	/*!
	 * 	@brief The overheadTimeCtrl function returns the additional delay when transmitting "size" bytes of control information. 
	 */
	double overheadTimeCtrl(int size);
	
	/*!
	 * 	@brief The overheadTimeData function returns the additional delay when transmitting size bytes of data information. 
	 */
	double overheadTimeData(int size);
	
private:
	
	double deviceComputationaldelay;	// Delay related to the embedded device running SUNSET 
	double deviceComputationalDelayCtrl;	// Delay related to the embedded device running SUNSET for control packet
	double deviceComputationalDelayData;	// Delay related to the embedded device running SUNSET for data packet
	
	double modemProcessingdelay;		// Delay related to the modem operations 
	double modemProcessingDelayCtrl;	// Delay related to the modem operations related to control packets
	double modemProcessingDelayData;	// Delay related to the modem operations related to data packets
	
	int baudRate;				// baudrate for serial line connection to the modem
	double dataRate;			// bit rate used for data packets
	double ctrlRate;			// bit rate used for control packets
	
	double maxPropagationDelay;		// maximum expected propoagation delay in the network
	double SIFS;				// delay for the Short InterFrame Space
	
	void setBaudRate(int bRate) { baudRate = bRate; }
	void setDeviceDelay(double dDelay) { deviceComputationaldelay = dDelay; }
	void setDataRate(double dRate) { dataRate = dRate; }
	void setCtrlRate(double cRate) { ctrlRate = cRate; }
	void setMaxPropagationDelay(double pDelay) { maxPropagationDelay = pDelay; }
	void setModemDelay(double mDelay) { modemProcessingdelay = mDelay; }
	void setSIFS(double sifs) { SIFS = sifs; }
	
};

#endif
