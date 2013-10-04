#include <tclcl.h>

extern EmbeddedTcl Sunset_Phy_Bellhop_TclCode;

extern "C" int Sunset_networking_phy_bellhop_Init() {
    Sunset_Phy_Bellhop_TclCode.load();
    return 0;
}

