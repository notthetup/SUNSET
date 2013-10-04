#include <tclcl.h>

extern EmbeddedTcl Sunset_Energy_Model_TclCode;

extern "C" int Sunset_core_energy_model_Init() {
    Sunset_Energy_Model_TclCode.load();
    return 0;
}
