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



#include <sunset_agent_pkt.h>
#include <iostream>

extern packet_t PT_SUNSET_AGT;

int hdr_Sunset_Agent::offset_;

static class Sunset_Agent_Class : public PacketHeaderClass 
{
public:
	
	Sunset_Agent_Class() : PacketHeaderClass("PacketHeader/Sunset_Agent", sizeof(hdr_Sunset_Agent)) {
		
		bind_offset(&hdr_Sunset_Agent::offset_);
		cout << "Agent size " << sizeof(hdr_Sunset_Agent)  << " offset " << hdr_Sunset_Agent::offset_ << endl;
		this->bind();
	}
	
} class_sunset_agent;
