static char code[] = "\n\
set maxPropagationDelay 1.0\n\
\n\
Sunset_Timing set deviceDelay_	0.0		;# 0.5 sec.\n\
Sunset_Timing set modemDelay_	0.0		;# 8 sec.\n\
Sunset_Timing set modemDelayCtrl_	0.0		;# 8 sec.\n\
Sunset_Timing set modemDelayData_	0.0		;# 8 sec.\n\
Sunset_Timing set dataRate_		500		;# 80bps\n\
Sunset_Timing set baudRate_		19200		;# 19200 baud\n\
Sunset_Timing set ctrlRate_		500		;# 80bps\n\
Sunset_Timing set pDelay_		$maxPropagationDelay\n\
Sunset_Timing set sifs_		0.00010	\n\
Sunset_Timing set slotTime_		0.000020\n\
\n\
Sunset_Timing set modemDelayCtrl_	0.0		;# 8 sec.\n\
Sunset_Timing set modemDelayData_	0.0		;# 8 sec.\n\
Sunset_Timing set deviceDelayCtrl_	0.0		;# 8 sec.\n\
Sunset_Timing set deviceDelayData_	0.0		;# 8 sec.\n\
\n\
";
#include "tclcl.h"
EmbeddedTcl Sunset_Timing_TclCode(code);
