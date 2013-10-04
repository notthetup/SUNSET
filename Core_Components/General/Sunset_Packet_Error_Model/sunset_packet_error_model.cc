/* SUNSET - Sapienza University Networking framework for underwater Simulation, Emulation and real-life Testing
 *
 * Copyright (C) 2012 Regents of UWSN Group of SENSES Lab
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
 * The author would like to thank Roberto Petroccia <petroccia@di.uniroma1.it>, which has contributed to the development of this module. 
 */


#include <sunset_packet_error_model.h>

Sunset_Packet_Error_Model* Sunset_Packet_Error_Model::per_model = NULL;

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_Packet_Error_ModelClass : public TclClass 
{
public:
	
	Sunset_Packet_Error_ModelClass() : TclClass("Module/Sunset_Packet_Error_Model") {}
	
	TclObject* create(int, const char*const*) {
		
		return (new Sunset_Packet_Error_Model());
	}
	
} class_Sunset_Packet_Error_ModelClass;

Sunset_Packet_Error_Model::Sunset_Packet_Error_Model() : TclObject()
{
	model = -1;
	
	PER = 0.0;
	RxSnrPenalty_dB_ = 0.0;
	
	per_model = this;	
}

/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script 
 *	@param[in] argc argc is a count of the arguments supplied to the command function.
 *	@param[in] argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_Packet_Error_Model::command( int argc, const char*const* argv ) 
{
	if ( argc == 4 ) {
		
		/* The packet error model can be set by "errorModel". */
		
		if (strcmp(argv[1], "errorModel") == 0) {
			
			/* If "BPSK" is chosen then the SNR penalty (in DB) on the reception must be set. */
			
			if ( strcasecmp("BPSK", argv[2]) == 0 ) {
				
				model = BPSK_MOD;
				RxSnrPenalty_dB_ = atof(argv[3]);
			}
			
			/* If "STATIC" is chosen then the static PER must be set. */
			
			else if ( strcasecmp("STATIC", argv[2]) == 0 ) {
				
				model = STATIC_MOD;
				PER = atof(argv[3]);
			}
			
			/* If "FSK" is chosen then the SNR penalty (in DB) on the reception must be set. */
			
			else if ( strcasecmp("FSK", argv[2]) == 0 ) {
				
				model = FSK_MOD;
				RxSnrPenalty_dB_ = atof(argv[3]);
			}
			else {
				
				Sunset_Debug::debugInfo(-1, -1, "Sunset_Packet_Error_Model::command Unknown Error Model");
				exit(1);
			}
			
			return TCL_OK;
		}
	}
	
	return TclObject::command(argc, argv);
}

/*!
 * 	@brief The instance function returns a packet error instance.
 */

Sunset_Packet_Error_Model* Sunset_Packet_Error_Model::instance() 
{
	if( per_model == NULL) {
		
		Sunset_Packet_Error_Model();
	}
	
	return per_model;
}

/*!
 * 	@brief The getPER function returns the Packet Error Rate according to the selected packet error model.
 *	@param snr The Signal-to-noise ratio.
 *  	@param nbits Packet size expressed in bits.
 * 	@retval PER Packet Error Rate.
 */

double Sunset_Packet_Error_Model::getPER(double snr, int nbits)
{
	switch( model ) {
			
		case BPSK_MOD:
			
			return getPER_bpsk(snr, nbits);
			
		case FSK_MOD:
			
			return getPER_fsk(snr, nbits);
			
		case STATIC_MOD:
			
			return getPER_static();
			
		default:
			
			Sunset_Debug::debugInfo(-1, -1, "Sunset_Packet_Error_Model::getPER No model defined - ERROR");
			exit(1);
	}
}

/*!
 * 	@brief The getPER_bpsk function returns the Packet Error Rate according to the BPSK modulation.
 *	@param snr The Signal-to-noise ratio.
 *  	@param nbits Packet size expressed in bits.
 * 	@retval PER Packet Error Rate.
 */

double Sunset_Packet_Error_Model::getPER_bpsk(double snr, int nbits)
{
	if ( model != BPSK_MOD ) {
		
		Sunset_Debug::debugInfo(-1, -1, "Sunset_Packet_Error_Model::getPER_bpsk BPSK model unexpected - ERROR");
		exit(1);
	}
	
	double snr_with_penalty = snr * pow(10, RxSnrPenalty_dB_/10.0);
	double ber = 0.5*erfc(sqrt(snr));  
	
	double per = 1-pow(1 - ber, nbits );
	
	return per;
}

/*!
 * 	@brief The getPER_bpsk function returns the Packet Error Rate according to the FSK modulation.
 *	@param snr The Signal-to-noise ratio.
 *  	@param nbits Packet size expressed in bits.
 * 	@retval PER Packet Error Rate.
 */

double Sunset_Packet_Error_Model::getPER_fsk(double snr, int nbits)
{
	if ( model != FSK_MOD ) {
		
		Sunset_Debug::debugInfo(-1, -1, "Sunset_Packet_Error_Model::getPER_fsk FSK model unexpected - ERROR");
		exit(1);
	}
	
	double snr_with_penalty = snr * pow(10, RxSnrPenalty_dB_/10.0);
	double ber = 0.5*erfc(sqrt(snr/2.0));  
	
	double per = 1-pow(1 - ber, nbits );
	
	return per;
}


/*!
 * 	@brief The getPER_static function returns a static PER.
 * 	@retval PER Packet Error Rate.
 */

double Sunset_Packet_Error_Model::getPER_static()
{
	if ( model != STATIC_MOD ) {
		
		Sunset_Debug::debugInfo(-1, -1, "Sunset_Packet_Error_Model::getPER_static STATIC model unexpected - ERROR");
		exit(1);
	}
	
	return PER;
}
