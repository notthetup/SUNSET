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

#include <poll.h>

#include <sunset_micro_modem_connection.h>

Sunset_MicroModem_Conn::Sunset_MicroModem_Conn(const char *devName,  int rate) : Sunset_Serial_Connection(devName, rate)
{
	
}

Sunset_MicroModem_Conn::~Sunset_MicroModem_Conn() {
}

/*!	@brief The open_connection function opens a serial line connection.  */

bool Sunset_MicroModem_Conn::open_connection() {
	
	Sunset_Debug::debugInfo(4, -1, "Sunset_MicroModem_Conn::open_connection connecting to %s baudRate %ld", serialDev, baudRate);
	
	return Sunset_Serial_Connection::open_connection();
}

/*!	@brief The open_connection function closes a serial line connection.  */

bool Sunset_MicroModem_Conn::close_connection() {
	
	Sunset_Debug::debugInfo(4, -1, "Sunset_MicroModem_Conn::close_connection close_connection to %s baudRate %ld", serialDev, baudRate);
	
	return Sunset_Serial_Connection::close_connection();
}

/*! 	@brief The um_poll function polls serial device to check for non-blocking read. 
 *	@param dev The device to be polled.
 * 	@retval > 0  if device is ready to be read.
 */

int Sunset_MicroModem_Conn::um_poll(int dev)
{
	int ret;
	struct pollfd pfd;
	
	pfd.fd      = dev;
	pfd.events  = POLLIN;
	pfd.revents = 0;
	
	//	poll works as select ... wait I/O on array of descpritor (first argument), 
	//	size of the array (second argument), timeout (third argument)
	
	ret = poll(&pfd, 1, 100000); 
	
	if (0 > ret) {
		
		perror("poll");
		
		return -1;
	}
	
	if (POLLIN & pfd.revents) 
		return 1;
	
	return 0;
}


/*!	@brief The read_data function reads from the serial line looking for packet delimiter 
 *	to extract the packet received by the modem which ahs to be provided to the upper layers.
 *	@param buf The buffer to be read.
 *	@param maxlen The maximal length of the buffer.
 * 	@retval The number of bytes correctly read.
 */

int Sunset_MicroModem_Conn::read_data(char * buf, int max_len) 
{
	int i = 0;
  	char ch;
	int start = 0;
	int res = 0;
	
	while (um_poll(fd)) {
		
		res = read (fd, &ch, 1);
		
		Sunset_Debug::debugInfo(6, -1, "Sunset_MicroModem_Conn::read_data start char i = %d - %d (%x)", i, ch, ch);
		
		if (ch == '$') {
			
			if (!start) {
				
				i = 0;
			}
			
			buf[i++] = ch;
			
			Sunset_Debug::debugInfo(6, -1, "Sunset_MicroModem_Conn::read_data start");
			
			if (start) {
				
				buf[i++] = '\0';
				
				Sunset_Debug::debugInfo(-1, -1, "Sunset_MicroModem_Conn::read_data set start to 1 twice ERROR %s", buf);
				
				start = 0;
				i = 0;
				
				buf[i++] = ch;
			}
			
			start = 1;
		}
		else if(ch == '\r') {
			
			if ( i >= max_len ) {
				
				Sunset_Debug::debugInfo(-1, -1, "Sunset_MicroModem_Conn::read_data too many characters have been read 1 ERROR");
				
				return 0;
			}
			
			buf[i++]=ch;
			
			res = read (fd, &ch, 1);
			
			Sunset_Debug::debugInfo(6, -1, "Sunset_MicroModem_Conn::read_data start char i = %d - %d (%x)", i, ch, ch);
			
			if(ch == '\n')  { /* end packet */
				
				if (i >= max_len) {
					
					Sunset_Debug::debugInfo(-1, -1, "Sunset_MicroModem_Conn::read_data too many characters have been read 2 ERROR");
					
					return 0;
				}
				
				buf[i++] = ch;
				
				if (!start) {
					
					Sunset_Debug::debugInfo(-1, -1, "Sunset_MicroModem_Conn::read_data set end with no start1 ERROR");
					
					return 0;
				}
				else {
					
					start = 0;
				}
				
				return i;
			}
			else if(ch == '\r') {
				
				if (i >= max_len) {
					
					Sunset_Debug::debugInfo(-1, -1, "Sunset_MicroModem_Conn::read_data too many characters have been read 3 ERROR");
					
					return 0;
				}
				
				buf[i++] = '\n';
				
				if (!start) {
					
					Sunset_Debug::debugInfo(-1, -1, "Sunset_MicroModem_Conn::read_data set end with no start2 ERROR");
					
					return 0;
				}
				else {
					
					start = 0;
				}
				
				return i;
			}
			else if (ch == '$') {
				
				if (!start) {
					
					i = 0;
				}
				
				buf[i++] = ch;
				
				Sunset_Debug::debugInfo(6, -1, "Sunset_MicroModem_Conn::read_data start2");
				
				if (start) {
					
					buf[i++] = '\0';
					
					Sunset_Debug::debugInfo(-1, -1, "Sunset_MicroModem_Conn::read_data set start2 to 1 twice ERROR %s", buf);
					
					start = 0;
					i = 0;
					
					buf[i++] = ch;
				}
				
				start = 1;
			}
			else {
				if (i >= max_len) {
					
					Sunset_Debug::debugInfo(-1, -1, "Sunset_MicroModem_Conn::read_data too many characters have been read 4 ERROR");
					
					return 0;
				}
				
				buf[i++] = ch;
			}
			
			
		}
		else if(ch == '\n') {
			
			if (i >= max_len) {
				
				Sunset_Debug::debugInfo(-1, -1, "Sunset_MicroModem_Conn::read_data too many characters have been read 5 ERROR");
				
				return 0;
			}
			
			buf[i++]=ch;
			
			res = read (fd, &ch, 1);
			
			Sunset_Debug::debugInfo(6, -1, "Sunset_MicroModem_Conn::read_data start char i = %d - %d (%x)", i, ch, ch);
			
			if(ch == '\n') { /* end packet */
				
				if (i >= max_len) {
					
					Sunset_Debug::debugInfo(-1, -1, "Sunset_MicroModem_Conn::read_data too many characters have been read 6 ERROR");
					
					return 0;
				}
				
				buf[i++] = ch;
				
				if (i-2 > 0) {
					
					buf[i-2] = '\r';
					
					if (!start) {
						
						Sunset_Debug::debugInfo(-1, -1, "Sunset_MicroModem_Conn::read_data set end with no start1 ERROR");
						
						return 0;
					}
					else {
						
						start = 0;
					}
					
					return i;
				}
				
				Sunset_Debug::debugInfo(-1, -1, "Sunset_MicroModem_Conn::read_data ERROR on indexes!!!");
				
				start = 0;
				
				return 0;
			}
			else if(ch == '\r') {
				
				if (i >= max_len) {
					
					Sunset_Debug::debugInfo(-1, -1, "Sunset_MicroModem_Conn::read_data too many characters have been read 7 ERROR");
					
					return 0;
				}
				
				buf[i++] = '\n';
				
				if (i-2 > 0) {
					
					buf[i-2] = '\r';
					
					if (!start) {
						
						Sunset_Debug::debugInfo(-1, -1, "Sunset_MicroModem_Conn::read_data set end with no start1 ERROR");
						
						return 0;
					}
					else {
						
						start = 0;
					}
					
					return i;
				}
				
				Sunset_Debug::debugInfo(-1, -1, "Sunset_MicroModem_Conn::read_data ERROR on indexes 2!!!");
				
				start = 0;
				
				return 0;
			}
			else if (ch == '$') {
				
				if (!start) {
					
					i = 0;
				}
				
				buf[i++] = ch;
				
				Sunset_Debug::debugInfo(6, -1, "Sunset_MicroModem_Conn::read_data start3");
				
				if (start) {
					
					buf[i++] = '\0';
					Sunset_Debug::debugInfo(-1, -1, "Sunset_MicroModem_Conn::read_data set start3 to 1 twice ERROR %s", buf);
					
					start = 0;
					i = 0;
					
					buf[i++] = ch;
				}
				
				start = 1;
			}
			else {
				if (i >= max_len) {
					
					Sunset_Debug::debugInfo(-1, -1, "Sunset_MicroModem_Conn::read_data too many characters have been read 8 ERROR");
					
					return 0;
				}
				
				buf[i++] = ch;
			}
		}
		else
		{
			if (i >= max_len) {
				
				Sunset_Debug::debugInfo(-1, -1, "Sunset_MicroModem_Conn::read_data too many characters have been read 9 ERROR");
				
				return 0;
			}
			
			buf[i++] = ch;
		}
	}
	
	return i;
}
