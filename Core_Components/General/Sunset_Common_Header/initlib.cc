#include <tclcl.h>

#include <sunset_common_pkt.h>

extern EmbeddedTcl Sunset_Common_Header_TclCode;

packet_t PT_SUNSET_COMMON;

extern "C" int Sunset_core_common_header_Init() {
    PT_SUNSET_COMMON = p_info::addPacket("SUNSET_COMMON");
    Sunset_Common_Header_TclCode.load();
    return 0;
}

