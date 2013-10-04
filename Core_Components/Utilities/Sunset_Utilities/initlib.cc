#include <tclcl.h>

extern EmbeddedTcl Sunset_Utilities_TclCode;

extern "C" int Sunset_core_utilities_Init() {
    Sunset_Utilities_TclCode.load();
    return 0;
}

