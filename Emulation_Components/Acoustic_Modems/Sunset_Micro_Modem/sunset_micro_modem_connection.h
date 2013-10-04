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


#ifndef __Sunset_MicroModem_Conn_h__
#define __Sunset_MicroModem_Conn_h__  

#include <sunset_serial.h>

#define MM_TIMEOUT_CONNECTION 5


/*! @brief This class is used for the connection to the Micro-Modem. A serial line connection is used.
 */

class Sunset_MicroModem_Conn  : public Sunset_Serial_Connection 
{
	
public:
	
	Sunset_MicroModem_Conn(const char* file, int baud);
	~Sunset_MicroModem_Conn();
	
public:
	int read_data(char *recvb, int max_len);
	virtual bool open_connection();
	virtual bool close_connection();
	
protected:
	int um_poll(int dev);
};

#endif
