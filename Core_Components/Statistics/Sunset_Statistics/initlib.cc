#include <tclcl.h>

extern EmbeddedTcl Sunset_Statistics_TclCode;

extern "C" int Sunset_core_statistics_Init() {
    Sunset_Statistics_TclCode.load();
    return 0;
}

