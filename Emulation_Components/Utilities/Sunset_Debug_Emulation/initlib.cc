#include <tclcl.h>

extern EmbeddedTcl Sunset_Debug_Emulation_TclCode;

extern "C" int Sunset_emulation_debug_emulation_Init() {
    Sunset_Debug_Emulation_TclCode.load();
    return 0;
}

