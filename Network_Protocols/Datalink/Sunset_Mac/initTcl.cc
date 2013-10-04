static char code[] = "\n\
Module/MMac/Sunset_Mac set moduleAddress -1\n\
Module/MMac/Sunset_Mac set runId -1\n\
Module/MMac/Sunset_Mac set MAC_HDR_SIZE 3\n\
PacketHeaderManager set tab_(PacketHeader/Sunset_Mac) 1\n\
";
#include "tclcl.h"
EmbeddedTcl Sunset_Mac_TclCode(code);
