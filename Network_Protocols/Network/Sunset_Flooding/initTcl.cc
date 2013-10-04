static char code[] = "\n\
Module/Sunset_Routing set moduleAddress -1\n\
Module/Sunset_Routing set debug_ false\n\
\n\
Module/Sunset_Flooding set moduleAddress -1\n\
Module/Sunset_Flooding set debug_ false\n\
\n\
Module/Sunset_Flooding set probability_ 1.0\n\
Module/Sunset_Flooding set max_forwards_ 5\n\
Module/Sunset_Flooding set max_random_time_ 0.0\n\
Module/Sunset_Flooding set debug_ false\n\
Module/Sunset_Flooding set moduleAddress -1\n\
";
#include "tclcl.h"
EmbeddedTcl Sunset_Flooding_TclCode(code);
