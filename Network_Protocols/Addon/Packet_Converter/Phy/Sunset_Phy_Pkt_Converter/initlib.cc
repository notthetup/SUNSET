#include <tclcl.h>


extern EmbeddedTcl Sunset_Phy_PktConverter_TclCode;

extern "C" int Sunset_networking_phy_pktconverter_Init() {
    Sunset_Phy_PktConverter_TclCode.load();
    return 0;
}

