#include <tclcl.h>


extern EmbeddedTcl Sunset_Agent_PktConverter_TclCode;

extern "C" int Sunset_networking_agent_pktconverter_Init() {
    Sunset_Agent_PktConverter_TclCode.load();
    return 0;
}

