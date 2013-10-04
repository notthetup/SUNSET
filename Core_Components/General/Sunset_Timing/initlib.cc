#include <tclcl.h>

extern EmbeddedTcl Sunset_Timing_TclCode;

extern "C" int Sunset_core_timing_Init() {
    Sunset_Timing_TclCode.load();
    return 0;
}

