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

/*
 * The author would like to thank Daniele Spaccini <spaccini@di.uniroma1.it>, which has contributed to the development of this module. 
 */

#ifndef __Sunset_RealTimeScheduler_h__
#define __Sunset_RealTimeScheduler_h__

#include <scheduler.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <sys/time.h>
#include <sunset_utilities.h>

#define S_EPSILON1 	0.00001
#define S_EPSILON2 	1e-3

/*! @brief This class implements the real-time scheduler, which is used when running in emulation mode. */

class Sunset_RealTimeScheduler : public CalendarScheduler {
	
public:
	Sunset_RealTimeScheduler();
	~Sunset_RealTimeScheduler();
	
	virtual void run();
	
	/*! @brief The start function returns the global time, epoch time, when the emulation started. */
	double start() const { return start_; }
	
	virtual void reset();
	void sync();		// sync emulation time accordig to epoch time
	void schedule(Handler*, Event*, double delay);	// schedule an even after a given delay
	double getEpoch();	// return the epoch time
	double getNOW();	//return the time from the beginnin of the test
	
protected:
	
	/*! @brief The getThreadCondition function returns the mutex variable used to signal when new events are added to the scheduler. */
	pthread_cond_t* getThreadCondition() { return &cond_mutex; }
	
	void dispatch(Event*);			// execute an event
	void dispatch(Event*, double);		// exec event, set clock_
	
	void waitEvent(struct timespec);	//wait an event for a given time
	double tod();				//return the emulation time in seconds
	double slop_;				// allowed drift between real-time and virt time
	double start_;				// starting time
	pthread_mutex_t mutex;
	pthread_mutex_t sched_mutex;
	pthread_mutex_t current_time_mutex;
	pthread_cond_t cond_mutex;
	
	double emulationStartTime;		//start time
	
	double getEpoch(struct timespec tv);
	
	struct timespec current_time;
	struct timespec next_time;
};

#endif
