#include <tclcl.h>

extern EmbeddedTcl Sunset_Micro_Modem_TclCode;

extern "C" int Sunset_emulation_micro_modem_Init() {
    Sunset_Micro_Modem_TclCode.load();
    return 0;
}

