#include <tclcl.h>

extern EmbeddedTcl Sunset_Csma_Aloha_TclCode;

extern "C" int Sunset_networking_csma_aloha_Init() {
    Sunset_Csma_Aloha_TclCode.load();
    return 0;
}

