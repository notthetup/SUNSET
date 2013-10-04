#include <tclcl.h>
#include <sunset_modem2phy-clmsg.h>

extern EmbeddedTcl Sunset_Modem_Phy_TclCode;

extern "C" int Sunset_core_modem_phy_Init() {
    Sunset_Modem_Phy_TclCode.load();
    return 0;
}

