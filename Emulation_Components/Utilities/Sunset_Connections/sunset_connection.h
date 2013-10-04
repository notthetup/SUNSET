/*/* SUNSET - Sapienza University Networking framework for underwater Simulation, Emulation and real-life Testing
 *
 * Copyright (C) 2012 Regents of UWSN Group of SENSES Lab <http://reti.dsi.uniroma1.it/SENSES_lab/>
 *
 * Author: Roberto Petroccia - petroccia@di.uniroma1.it
 *         Daniele Spaccini - spaccini@di.uniroma1.it
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

#ifndef __Sunset_Connection_h__
#define __Sunset_Connection_h__

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <stdarg.h>
#include <string>
#include <list>
#include <map>
#include <iostream>
#include <stack>
#include <pthread.h>
#include <queue>
#include <sys/types.h>   /* predefined types */
#include <unistd.h>      /* include unix standard library */
#include <arpa/inet.h>   /* IP addresses conversion utilities */
#include <sys/socket.h>  /* socket library */
#include <netinet/in.h>

#include <sunset_debug.h>

/*! \brief The generic connection class. The TCP, UDP and Serial classes all extend this generic class.  */

class Sunset_Connection {
	
public:
	
	Sunset_Connection() { fd = -1; };
	~Sunset_Connection() {};
	
	virtual bool open_connection() = 0;	//open a connection on the selected line
	
	virtual bool close_connection() = 0;	//close a connection on the selected line
	
	virtual int read_data(char *, int max_len) = 0;	
	
	virtual int read_data(int fd, char *, int max_len) = 0; //read data from the line with the given file descriptor
	
	virtual bool write_data(char * data, int len_data) = 0;
	
	virtual bool write_data(int fd, char * data, int len_data) = 0; //write data to the line with the given file descriptor
	
	inline int get_fd() { return fd; }	//return the file descriptor for this connection
	
protected:
	
	int fd;
};

#endif
