#include <tclcl.h>
#include <sunset_routing_pkt.h>

extern EmbeddedTcl Sunset_Routing_TclCode;

packet_t PT_SUNSET_RTG;

extern "C" int Sunset_networking_routing_Init() {
   PT_SUNSET_RTG = p_info::addPacket("SUNSET_RTG");
    Sunset_Routing_TclCode.load();
    return 0;
}

