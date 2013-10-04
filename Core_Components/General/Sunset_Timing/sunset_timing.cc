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

#include <sunset_timing.h>

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_TimingClass : public TclClass 
{
public:
	
	Sunset_TimingClass() : TclClass("Sunset_Timing") {}
	
	TclObject* create(int, const char*const*) {
		
		return (new Sunset_Timing());
	}
	
} class_Sunset_TimingClass;


/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script 
 *	@param argc argc is a count of the arguments supplied to the command function.
 *	@param argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_Timing::command( int argc, const char*const* argv ) 
{
	if ( argc == 3 ) {
		
		/* Set the baudrate assumed to compute the delay on trasfering information when using serial line connections*/
		
		if (strcmp(argv[1], "setBaudRate") == 0) {
			
			setBaudRate(atoi(argv[2]));
			
			return TCL_OK;
		}	
		
		/* Set the average computation delay to be considered when running the framework on an embedded device.*/
		
		if (strcmp(argv[1], "setDeviceDelay") == 0) {
			
			setDeviceDelay(atof(argv[2]));
			
			return TCL_OK;
		}	
		
		/* Set the assumed bit rate for data packets.*/
		
		if (strcmp(argv[1], "setDataRate") == 0) {
			
			setDataRate(atof(argv[2]));
			
			return TCL_OK;
		}	
		
		/* Set the assumed bit rate for control packets.*/
		
		if (strcmp(argv[1], "setCtrlRate") == 0) {
			
			setCtrlRate(atof(argv[2]));
			
			return TCL_OK;
		}	
		
		/* Set the average delay related to acoustic modem operations when transmitting a packet.*/
		
		if (strcmp(argv[1], "setModemDelay") == 0) {
			
			setModemDelay(atof(argv[2]));
			
			return TCL_OK;
		}	
		
		/* Set the maximum propoagation delay assumed in the network.*/
		
		if (strcmp(argv[1], "setMaxPropagationDelay") == 0) {
			
			setMaxPropagationDelay(atof(argv[2]));
			
			return TCL_OK;
		}	
	}	
	
	return TclObject::command(argc, argv);
}

Sunset_Timing::Sunset_Timing() : TclObject() 
{
	int baudR = 0;
	
	deviceComputationaldelay = 0.0;
	deviceComputationalDelayCtrl = 0.0;
	deviceComputationalDelayData = 0.0;
	modemProcessingdelay = 0.0;
	baudRate = 0;
	dataRate = 0.0;
	ctrlRate = 0.0;
	
	bind("deviceDelay_", &deviceComputationaldelay);
	bind("deviceDelayCtrl_", &deviceComputationalDelayCtrl);
	bind("deviceDelayData_", &deviceComputationalDelayData);
	bind("modemDelay_", &modemProcessingdelay);
	bind("modemDelayCtrl_", &modemProcessingDelayCtrl);
	bind("modemDelayData_", &modemProcessingDelayData);
	bind("dataRate_", &dataRate);
	bind("ctrlRate_", &ctrlRate);
	bind("baudRate_", &baudR);
	bind("pDelay_", &maxPropagationDelay);
	bind("sifs_", &SIFS);
	baudRate = baudR;
	
	Sunset_Debug::debugInfo(4, -1, "Sunset_Timing::Sunset_Timing()");
}

/*!
 * 	@brief The txtime function computes the time needed to transmit a given number of bytes using the bit rate in input.
 * 	@param[in] size Number of bytes to be transmitted.
 * 	@param[in] rate The bit rate.
 * 	@retval The transmission delay.
 */

double Sunset_Timing::txtime(int size, double rate)
{
	return ((double)size * 8.0)/(rate);
}


/*!
 * 	@brief The overheadTime function returns the additional delay when transmitting "size" bytes of data. 
 *  It consider the delay related to the use of embedded device, acoustic modem, and delay to transfer the bytes of information to the external device.
 * 	@param[in] size Number of bytes to be transmitted.
 * 	@retval The additional delay.
 */

double Sunset_Timing::overheadTime(int size)
{
	Sunset_Debug::debugInfo(4, -1, "Sunset_Timing::overheadTime size %d devDelay %f modemDelay %f transferTime %f", 
				size, getDeviceDelay(), getModemDelay(), transfertTime(size) );
	return getDeviceDelay() + getModemDelay() + transfertTime(size);
}

/*!
 * 	@brief The overheadTimeCtrl function returns the additional delay when transmitting "size" bytes of control information. 
 *  It consider the delay related to the use of embedded device, acoustic modem, and delay to transfer the bytes of control information to the external device.
 * 	@param[in] size Number of control bytes to be transmitted.
 * 	@retval The additional delay.
 */
double Sunset_Timing::overheadTimeCtrl(int size)
{
	return getDeviceDelayCtrl() + getModemDelayCtrl() + transfertTime(size);
}

/*!
 * 	@brief The overheadTimeData function returns the additional delay when transmitting size bytes of data information. 
 *  It consider the delay related to the use of embedded device, acoustic modem, and delay to transfer the bytes of data information to the external device.
 * 	@param[in] size Number of data bytes to be transmitted.
 * 	@retval The additional delay.
 */
double Sunset_Timing::overheadTimeData(int size)
{
	return getDeviceDelayData() + getModemDelayData() + transfertTime(size);
}

/*!
 * 	@brief The txtime function computes the time needed to transmit a given number of bytes using the bit rate associated to data or control information. This specifc function is called when running in simulation mode. 
 *  When running in emulation mode the timing emulation class and functions are used.
 * 	@param[in] size Number of bytes to be transmitted.
 * 	@param[in] rate The rate type, it is data or control.
 * 	@retval The transmission delay.
 */

double Sunset_Timing::txtime(int size, sunset_rateType rate)
{	
	double auxRate;
	
	Sunset_Debug::debugInfo(4, -1, "Sunset_Timing::txtime size %d", size);
	
	switch (rate) {
			
		case TIMING_DATA_RATE:
			auxRate = getDataRate();
			break;
			
		case TIMING_CTRL_RATE:
			auxRate = getCtrlRate();
			break;
			
		default:
			break;
	}
	
	Sunset_Debug::debugInfo(4, -1, "Sunset_Timing::txtime size %d auxRate %f", size, auxRate);
	
	return ((double)size * 8.0)/(auxRate);
}

/*!
 * 	@brief The transfertTime function computes the time needed to transfer a given number of bytes to an external device using the serial line.
 * 	@param[in] size Number of bytes to be transferred.
 * 	@retval The transfer delay.
 */
double Sunset_Timing::transfertTime(int size)
{
	Sunset_Debug::debugInfo(4, -1, "Sunset_Timing::transfertTime size %d", size);
	
	return (size * BAUD_CONV) / (double)baudRate;
}

/*!
 * 	@brief The getPktSize function return the packet size used by the external device when transmitting size bytes of information. This specifc function is called when running in simulation mode. When running in emulation mode the timing emulation class and functions are used.
 * 	@param[in] dataSize Number of bytes to be transmitted.
 * 	@retval The number of bytes to be transmitted.
 */
int Sunset_Timing::getPktSize(int dataSize)
{
	Sunset_Debug::debugInfo(4, -1, "Sunset_Timing::getPktSize size %d", dataSize);
	
	return Sunset_Utilities::get_pkt_size(dataSize);
}


/*!
 * 	@brief The getPktSize function returns the packet size used by the external device when transmitting packet p. 
 *  This specifc function is called when running in simulation mode. When running in emulation mode the timing emulation class and functions are used.
 * 	@param[in] p Packet to be transmitted.
 * 	@retval The number of bytes to be transmitted. 
 */
int Sunset_Timing::getPktSize(Packet* p)
{
	Sunset_Debug::debugInfo(4, -1, "Sunset_Timing::getPktSize size_pkt %d", HDR_CMN(p)->size());
	
	return Sunset_Utilities::get_pkt_size(p);
}
