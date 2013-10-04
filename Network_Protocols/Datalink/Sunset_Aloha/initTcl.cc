static char code[] = "\n\
Module/MMac/Sunset_Mac set moduleAddress -1\n\
Module/MMac/Sunset_Mac set runId -1\n\
Module/MMac/Sunset_Mac set MAC_HDR_SIZE 3\n\
\n\
Module/MMac/Sunset_Aloha set moduleAddress -1\n\
Module/MMac/Sunset_Aloha set runId -1\n\
Module/MMac/Sunset_Aloha set MAC_HDR_SIZE 3\n\
";
#include "tclcl.h"
EmbeddedTcl Sunset_Aloha_TclCode(code);
