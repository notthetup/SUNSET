#include <tclcl.h>

extern EmbeddedTcl Sunset_Flooding_TclCode;

extern "C" int Sunset_networking_flooding_Init() {
    Sunset_Flooding_TclCode.load();
    return 0;
}

