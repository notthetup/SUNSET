static char code[] = "\n\
Module/Sunset_Phy_Urick set TxPower_	5e9\n\
Module/Sunset_Phy_Urick set NoiseSPD_	[ expr 1.28e-23 * 300 ]\n\
Module/Sunset_Phy_Urick set MaxTxSPL_dB_              190\n\
Module/Sunset_Phy_Urick set MinTxSPL_dB_              10\n\
Module/Sunset_Phy_Urick set RxSnrPenalty_dB_  	      -10\n\
Module/Sunset_Phy_Urick set TxSPLMargin_dB_ 	        10\n\
Module/Sunset_Phy_Urick set ConsumedEnergy_ 	        0\n\
Module/Sunset_Phy_Urick set debug_	                  0\n\
Module/Sunset_Phy_Urick set SPLOptimization_          0\n\
Module/Sunset_Phy_Urick set CentralFreqOptimization_  0\n\
Module/Sunset_Phy_Urick set BandwidthOptimization_    0\n\
Module/Sunset_Phy_Urick set MaxTxRange_           10000\n\
Module/Sunset_Phy_Urick set PER_target_            0.01\n\
\n\
Module/Sunset_Phy_Urick set use_pkt_error_ 0\n\
Module/Sunset_Phy_Urick set use_energy_ 0";
#include "tclcl.h"
EmbeddedTcl Sunset_Phy_Urick_TclCode(code);
