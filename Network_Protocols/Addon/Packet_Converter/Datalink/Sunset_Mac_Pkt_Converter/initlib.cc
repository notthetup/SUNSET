#include <tclcl.h>


extern EmbeddedTcl Sunset_Mac_PktConverter_TclCode;

extern "C" int Sunset_networking_mac_pktconverter_Init() {
    Sunset_Mac_PktConverter_TclCode.load();
    return 0;
}

