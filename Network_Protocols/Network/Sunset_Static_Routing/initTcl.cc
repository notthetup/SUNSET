static char code[] = "\n\
Module/Sunset_Routing set moduleAddress -1\n\
Module/Sunset_Routing set debug_ false\n\
\n\
Module/Sunset_Static_Routing set moduleAddress -1\n\
Module/Sunset_Static_Routing set debug_ false\n\
\n\
";
#include "tclcl.h"
EmbeddedTcl Sunset_Static_Routing_TclCode(code);
