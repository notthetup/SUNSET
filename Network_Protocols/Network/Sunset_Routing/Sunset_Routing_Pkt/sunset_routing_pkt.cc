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



#include <sunset_routing_pkt.h>
#include <iostream>

extern packet_t PT_SUNSET_RTG;

int hdr_Sunset_Routing::offset_;

static class Sunset_Routing_Class : public PacketHeaderClass 
{
public:
	
	Sunset_Routing_Class() : PacketHeaderClass("PacketHeader/Sunset_Routing", sizeof(hdr_Sunset_Routing)) {
		
		bind_offset(&hdr_Sunset_Routing::offset_);
		cout << "Routing size " << sizeof(hdr_Sunset_Routing)  << " offset " << hdr_Sunset_Routing::offset_ << endl;
		this->bind();
	}
	
} class_sunset_routing;
