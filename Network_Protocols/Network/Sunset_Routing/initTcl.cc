static char code[] = "\n\
Module/Sunset_Routing set moduleAddress -1\n\
Module/Sunset_Routing set debug_ false\n\
\n\
PacketHeaderManager set tab_(PacketHeader/Sunset_Routing) 1\n\
";
#include "tclcl.h"
EmbeddedTcl Sunset_Routing_TclCode(code);
