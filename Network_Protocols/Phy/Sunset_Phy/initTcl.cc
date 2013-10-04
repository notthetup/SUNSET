static char code[] = "\n\
Module/MPhy/Sunset_Phy set moduleAddress -1\n\
Module/MPhy/Sunset_Phy set use_pkt_error_ 0\n\
Module/MPhy/Sunset_Phy set use_ch_emu 0";
#include "tclcl.h"
EmbeddedTcl Sunset_Phy_TclCode(code);
