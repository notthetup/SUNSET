#include <tclcl.h>

extern EmbeddedTcl Sunset_Trace_TclCode;

extern "C" int Sunset_core_trace_Init() {
    Sunset_Trace_TclCode.load();
    return 0;
}

