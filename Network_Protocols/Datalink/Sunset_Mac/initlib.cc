#include <tclcl.h>

#include "sunset_mac_pkt.h"

extern EmbeddedTcl Sunset_Mac_TclCode;
packet_t PT_SUNSET_MAC;

extern "C" int Sunset_networking_mac_Init() {
    PT_SUNSET_MAC = p_info::addPacket("SUNSET_MAC");
    Sunset_Mac_TclCode.load();
    return 0;
}

