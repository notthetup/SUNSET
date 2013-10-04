#include <tclcl.h>

extern EmbeddedTcl Sunset_Aloha_TclCode;

extern "C" int Sunset_networking_aloha_Init() {
    Sunset_Aloha_TclCode.load();
    return 0;
}

