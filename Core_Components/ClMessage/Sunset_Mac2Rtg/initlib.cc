#include <tclcl.h>
#include <sunset_mac2rtg-clmsg.h>


extern EmbeddedTcl Sunset_Mac_Routing_TclCode;

extern "C" int Sunset_core_mac_routing_Init() {
    Sunset_Mac_Routing_TclCode.load();
    return 0;
}

