#include <tclcl.h>

extern EmbeddedTcl Sunset_Evologics_v_one_four_TclCode;

extern "C" int Sunset_emulation_evologics_v_one_four_Init() {
    Sunset_Evologics_v_one_four_TclCode.load();
    return 0;
}

