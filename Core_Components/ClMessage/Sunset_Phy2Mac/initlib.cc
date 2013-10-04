#include <tclcl.h>
#include <sunset_phy2mac-clmsg.h>

extern EmbeddedTcl Sunset_Phy_Mac_TclCode;

extern "C" int Sunset_core_phy_mac_Init() {
    Sunset_Phy_Mac_TclCode.load();
    return 0;
}

