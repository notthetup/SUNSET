#include <tclcl.h>


extern EmbeddedTcl Sunset_Queue_TclCode;

extern "C" int Sunset_core_queue_Init() {
    Sunset_Queue_TclCode.load();
    return 0;
}

