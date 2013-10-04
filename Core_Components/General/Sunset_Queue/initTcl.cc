static char code[] = "\n\
Queue/Sunset_Queue set		drop_front_		1\n\
Queue/Sunset_Queue set		queue_in_bytes_ 1\n\
Queue/Sunset_Queue set		qlimBytes	50000\n\
\n\
Queue/Sunset_Queue set		limit_			50\n\
\n\
Queue/Sunset_Queue set moduleAddress -1\n\
Queue/Sunset_Queue set debug_ false\n\
";
#include "tclcl.h"
EmbeddedTcl Sunset_Queue_TclCode(code);
