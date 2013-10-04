#include <tclcl.h>


extern EmbeddedTcl Sunset_Cbr_PktConverter_TclCode;

extern "C" int Sunset_networking_cbr_pktconverter_Init() {
    Sunset_Cbr_PktConverter_TclCode.load();
    return 0;
}

