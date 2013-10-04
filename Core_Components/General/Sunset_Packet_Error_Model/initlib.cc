#include <tclcl.h>

extern EmbeddedTcl Sunset_Packet_Error_Model_TclCode;

extern "C" int Sunset_core_packet_error_model_Init() {
    Sunset_Packet_Error_Model_TclCode.load();
    return 0;
}

