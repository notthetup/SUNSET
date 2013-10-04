#include <tclcl.h>

extern EmbeddedTcl Sunset_Phy_TclCode;

extern "C" int Sunset_networking_phy_Init() {
    Sunset_Phy_TclCode.load();
    return 0;
}

