static char code[] = "Module/Sunset_Phy_Bellhop set TxPower_	5e9\n\
Module/Sunset_Phy_Bellhop set NoiseSPD_	[ expr 1.28e-23 * 300 ]\n\
Module/Sunset_Phy_Bellhop set MaxTxSPL_dB_              190\n\
Module/Sunset_Phy_Bellhop set MinTxSPL_dB_              10\n\
Module/Sunset_Phy_Bellhop set RxSnrPenalty_dB_  	      -10\n\
Module/Sunset_Phy_Bellhop set TxSPLMargin_dB_ 	        10\n\
Module/Sunset_Phy_Bellhop set ConsumedEnergy_ 	        0\n\
Module/Sunset_Phy_Bellhop set debug_	                  0\n\
Module/Sunset_Phy_Bellhop set SPLOptimization_          0\n\
Module/Sunset_Phy_Bellhop set CentralFreqOptimization_  0\n\
Module/Sunset_Phy_Bellhop set BandwidthOptimization_    0\n\
Module/Sunset_Phy_Bellhop set MaxTxRange_           10000\n\
Module/Sunset_Phy_Bellhop set PER_target_            0.01\n\
\n\
\n\
Module/Sunset_Phy_Bellhop set use_pkt_error_ 0\n\
Module/Sunset_Phy_Bellhop set use_energy_ 0";
#include "tclcl.h"
EmbeddedTcl Sunset_Phy_Bellhop_TclCode(code);
