#include <tclcl.h>

extern EmbeddedTcl Sunset_Debug_TclCode;

extern "C" int Sunset_core_debug_Init() {
    Sunset_Debug_TclCode.load();
    return 0;
}

