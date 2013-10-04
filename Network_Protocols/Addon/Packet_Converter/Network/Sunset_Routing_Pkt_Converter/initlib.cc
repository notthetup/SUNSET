#include <tclcl.h>


extern EmbeddedTcl Sunset_Routing_PktConverter_TclCode;

extern "C" int Sunset_networking_routing_pktconverter_Init() {
    Sunset_Routing_PktConverter_TclCode.load();
    return 0;
}

