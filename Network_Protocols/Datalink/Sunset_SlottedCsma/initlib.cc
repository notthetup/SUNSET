#include <tclcl.h>

extern EmbeddedTcl Sunset_Slotted_Csma_TclCode;

extern "C" int Sunset_networking_slotted_csma_Init() {

	Sunset_Slotted_Csma_TclCode.load();
	
	return 0;
}

