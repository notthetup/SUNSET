#include <tclcl.h>


extern EmbeddedTcl Sunset_Module_TclCode;

extern "C" int Sunset_core_module_Init() {
    Sunset_Module_TclCode.load();
    return 0;
}

