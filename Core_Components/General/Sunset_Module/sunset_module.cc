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

#include "sunset_module.h"

/*!
 * 	@brief The start() function can be called from the TCL script to execute sunset module initialization operations when the simulation/emulation starts. It is always better to initialize references to the other classes in the start function, instead in the class constructor since the start function is called after the simulation starts and all other simulation objects have been already created.
 */

void Sunset_Module::start()
{    
	return;
}

/*!
 * 	@brief The stop() function can be called from the TCL script to execute sunset module operations when the simulation/emulation stops.
 */

void Sunset_Module::stop()
{
	return;
}
