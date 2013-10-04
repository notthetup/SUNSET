/* SUNSET - Sapienza University Networking framework for underwater Simulation, Emulation and real-life Testing
 *
 * Copyright (C) 2012 Regents of UWSN Group of SENSES Lab <http://reti.dsi.uniroma1.it/SENSES_lab/>
 *
 * Author: Daniele Spaccini - spaccini@di.uniroma1.it
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

/*
 * A special thanks go to Roberto Petroccia <petroccia@di.uniroma1.it> which has contributed to the development of this module. 
 */

#ifndef __Sunset_Channel_Emulator_Conn_h__
#define __Sunset_Channel_Emulator_Conn_h__  

#include <sunset_tcp.h>
#include <sunset_tcp_server.h>

/*! @brief This class is used to start and handle a channel emulator server connection.
 */

class Sunset_Channel_Emulator_Conn : public Sunset_Tcp_Server_Connection
{
	
public:
	
	Sunset_Channel_Emulator_Conn(const char* file, int baud);
	~Sunset_Channel_Emulator_Conn();
	
	virtual int read_data(int fd, char *recvb, int max_len);
};

#endif
