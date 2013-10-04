#include <tclcl.h>

extern EmbeddedTcl Sunset_Protocol_Statistics_TclCode;

extern "C" int Sunset_networking_protocol_statistics_Init() {
    Sunset_Protocol_Statistics_TclCode.load();
    return 0;
}

