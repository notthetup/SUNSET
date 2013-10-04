

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

Module/MPhy/BPSK/Underwater  set debug_                     0
Module/MPhy/BPSK/Underwater  set AcquisitionThreshold_dB_   $params(detectionDB)
Module/MPhy/BPSK/Underwater  set BitRate_                   $params(bitrate)
Module/MPhy/BPSK/Underwater  set MaxTxSPL_dB_               $params(txPower)
Module/MPhy/BPSK/Underwater  set MinTxSPL_dB_               10
Module/MPhy/BPSK/Underwater  set MaxTxRange_                $params(txRadius)
Module/MPhy/BPSK/Underwater  set PER_target_                0.1
Module/MPhy/BPSK/Underwater  set TxSPLMargin_dB_            10
Module/MPhy/BPSK/Underwater  set RxSnrPenalty_dB_           -10.0
Module/MPhy/BPSK/Underwater  set SPLOptimization_           0
Module/MPhy/BPSK/Underwater  set CentralFreqOptimization_   0
Module/MPhy/BPSK/Underwater  set BandwidthOptimization_     0

##########################################################################################
