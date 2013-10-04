#include <tclcl.h>

extern EmbeddedTcl Sunset_Evologics_v_one_six_TclCode;

extern "C" int Sunset_emulation_evologics_v_one_six_Init() {
    Sunset_Evologics_v_one_six_TclCode.load();
    return 0;
}

