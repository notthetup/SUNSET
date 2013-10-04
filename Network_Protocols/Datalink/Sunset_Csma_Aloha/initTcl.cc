static char code[] = "\n\
Module/MMac/Sunset_Mac set moduleAddress 	-1\n\
Module/MMac/Sunset_Mac set debug_		false\n\
Module/MMac/Sunset_Mac set MAC_HDR_SIZE		3\n\
\n\
Module/MMac/Sunset_Csma_Aloha set debug_ 		false\n\
Module/MMac/Sunset_Csma_Aloha set longRetryLimit	4\n\
Module/MMac/Sunset_Csma_Aloha set MAC_HDR_SIZE		3\n\
";
#include "tclcl.h"
EmbeddedTcl Sunset_Csma_Aloha_TclCode(code);
