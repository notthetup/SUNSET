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


#ifndef __Sunset_Serial_h__
#define __Sunset_Serial_h__

#include "sunset_connection.h"
#include <sunset_debug.h>

#define SUNSET_SERIAL_FILE_LEN		256
#define SUNSET_SERIAL_MAX_BUF_SIZE	32768

/*! \brief Serial connections class */

class Sunset_Serial_Connection : public Sunset_Connection {
	
public:
	
	Sunset_Serial_Connection(const char*, int);
	~Sunset_Serial_Connection();
	
	virtual bool open_connection();	//open the serial line connection
	
	virtual bool close_connection(); //close the serial line connection 
	
	virtual int read_data(char *, int);
	
	virtual int read_data(int fd, char *, int);
	
	virtual bool write_data(char * data, int len_data);
	
	virtual bool write_data(int fd, char * data, int len_data);
	
	speed_t getSBaudRate() { return sbaudRate; }	//return the baudrate
	
	int getIntBaudRate() { return baudRate; }	//return the integer conversion of the baudraet for debug
	
	virtual bool initialize();	//initialize the serial line
	
	char* getDevName() { return serialDev; }	//return the used serial interface
	
protected:
	
	char serialDev[SUNSET_SERIAL_FILE_LEN];		//serial interface used
	
	speed_t sbaudrate(int rate);
	
	speed_t sbaudRate;
	
	int baudRate;
};

#endif
