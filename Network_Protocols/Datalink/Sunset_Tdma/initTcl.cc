static char code[] = "\n\
Module/MMac/Sunset_Mac set moduleAddress -1\n\
Module/MMac/Sunset_Mac set debug_ false\n\
Module/MMac/Sunset_Mac set MAC_HDR_SIZE	3\n\
\n\
Module/MMac/Sunset_Tdma set debug_ false\n\
Module/MMac/Sunset_Tdma set longRetryLimit	4\n\
Module/MMac/Sunset_Tdma set shortRetryLimit	7\n\
Module/MMac/Sunset_Tdma set MAC_HDR_SIZE	3\n\
Module/MMac/Sunset_Tdma set ACK_SIZE	3\n\
Module/MMac/Sunset_Tdma set DATA_SIZE	32\n\
Module/MMac/Sunset_Tdma set use_ack_	0\n\
Module/MMac/Sunset_Tdma set slotTime_ 0.0\n\
\n\
Module/MMac/Sunset_Tdma set debug_ false\n\
Module/MMac/Sunset_Tdma set slot_per_frame_	16\n\
Module/MMac/Sunset_Tdma set slot_offset_	0\n\
\n\
";
#include "tclcl.h"
EmbeddedTcl Sunset_Tdma_TclCode(code);
