#include <tclcl.h>

extern EmbeddedTcl Sunset_Transport_TclCode;

extern "C" int Sunset_networking_transport_Init() {
    Sunset_Transport_TclCode.load();
    return 0;
}

