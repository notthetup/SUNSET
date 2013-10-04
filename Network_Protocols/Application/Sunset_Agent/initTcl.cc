static char code[] = "\n\
Module/Sunset_Agent set moduleAddress -1\n\
Module/Sunset_Agent set runId -1\n\
Module/Sunset_Agent set debug_ false\n\
Module/Sunset_Agent set DATA_SIZE 32\n\
Module/Sunset_Agent set portNumber -1\n\
Module/Sunset_Agent set start_pktId 0\n\
\n\
\n\
PacketHeaderManager set tab_(PacketHeader/Sunset_Agent) 1\n\
";
#include "tclcl.h"
EmbeddedTcl Sunset_Agent_TclCode(code);
