#include <tclcl.h>


extern EmbeddedTcl Sunset_Information_Dispatcher_TclCode;

extern "C" int Sunset_core_information_dispatcher_Init() {
    Sunset_Information_Dispatcher_TclCode.load();
    return 0;
}

