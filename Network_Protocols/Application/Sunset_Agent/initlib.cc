#include <tclcl.h>

#include <sunset_agent_pkt.h>

extern EmbeddedTcl Sunset_Agent_TclCode;

packet_t PT_SUNSET_AGT;

extern "C" int Sunset_networking_agent_Init() {
    PT_SUNSET_AGT = p_info::addPacket("SUNSET_AGT");
    Sunset_Agent_TclCode.load();
    return 0;
}

