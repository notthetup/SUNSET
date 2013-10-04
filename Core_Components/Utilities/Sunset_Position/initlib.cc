#include <tclcl.h>
#include "sunset_position.h"

extern EmbeddedTcl Sunset_position_TclCode;

extern "C" int Sunset_core_position_Init()
{
	Sunset_position_TclCode.load();
	return 0;
}
