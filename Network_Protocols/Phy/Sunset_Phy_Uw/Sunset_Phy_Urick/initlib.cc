#include <tclcl.h>

extern EmbeddedTcl Sunset_Phy_Urick_TclCode;

extern "C" int Sunset_networking_phy_urick_Init() {
    Sunset_Phy_Urick_TclCode.load();
    return 0;
}

