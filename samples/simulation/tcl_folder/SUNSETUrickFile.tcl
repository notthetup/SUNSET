

################################## WOSS SETTING #############################
WOSS/Utilities set debug 0
set woss_utilities [new "WOSS/Utilities"]

MPropagation/Underwater set windspeed_ $params(wind)
MPropagation/Underwater set shipping_  $params(ship)	

Module/UnderwaterChannel set debug_                    0

set channel [new "Module/UnderwaterChannel"]

MPropagation/Underwater set debug_ 0
set propagation [new "MPropagation/Underwater"]

set data_mask [new "MSpectralMask/Rect"]
$data_mask setFreq       $params(freq)
$data_mask setBandwidth  $params(bw)

Module/Sunset_Phy_Urick  set debug_                     0
Module/Sunset_Phy_Urick  set BitRate_                   $params(bitrate)
Module/Sunset_Phy_Urick  set MaxTxSPL_dB_               $params(txPower)
Module/Sunset_Phy_Urick  set MinTxSPL_dB_               10
Module/Sunset_Phy_Urick  set MaxTxRange_                50000
Module/Sunset_Phy_Urick  set PER_target_                0.001
Module/Sunset_Phy_Urick  set TxSPLMargin_dB_            10
Module/Sunset_Phy_Urick  set RxSnrPenalty_dB_           0.0
Module/Sunset_Phy_Urick  set SPLOptimization_           0
Module/Sunset_Phy_Urick  set CentralFreqOptimization_   0
Module/Sunset_Phy_Urick  set BandwidthOptimization_     0
Module/Sunset_Phy_Urick  set AcquisitionThreshold_dB_  1

##########################################################################################
