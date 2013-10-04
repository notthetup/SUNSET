#include <tclcl.h>

extern EmbeddedTcl Sunset_Static_Routing_TclCode;


extern "C" int Sunset_networking_static_routing_Init() {
    Sunset_Static_Routing_TclCode.load();
    return 0;
}

