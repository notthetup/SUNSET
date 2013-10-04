#include <tclcl.h>

extern EmbeddedTcl Sunset_Tdma_TclCode;

extern "C" int Sunset_networking_tdma_Init() {

	Sunset_Tdma_TclCode.load();
	return 0;
}

