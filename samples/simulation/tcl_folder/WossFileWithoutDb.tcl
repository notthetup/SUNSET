################################## WOSS SETTING #############################
set params(db_res_path)			"."

WOSS/MPropagation set windspeed_ $params(wind)
WOSS/MPropagation set shipping_  $params(ship)	

WOSS/Definitions/RandomGenerator/NS2 set rep_number_ $params(rep_num)
WOSS/Definitions/RandomGenerator/C   set seed_       $params(rep_num)

set ssp_creator         [new "WOSS/Definitions/SSP"]
set sediment_creator    [new "WOSS/Definitions/Sediment"]
set pressure_creator    [new "WOSS/Definitions/Pressure"]
set time_arr_creator    [new "WOSS/Definitions/TimeArr"]
set time_reference      [new "WOSS/Definitions/TimeReference/NS2"]
set transducer_creator  [new "WOSS/Definitions/Transducer"]
set altimetry_creator   [new "WOSS/Definitions/Altimetry/Bretschneider"] ;#This line is needed for WOSS version newer than "woss-1.2.0"
# set rand_generator      [new "WOSS/Definitions/RandomGenerator/NS2"]
set rand_generator      [new "WOSS/Definitions/RandomGenerator/C"]
$rand_generator initialize

set def_handler [new "WOSS/Definitions/Handler"]
$def_handler setSSPCreator         $ssp_creator
$def_handler setSedimentCreator    $sediment_creator
$def_handler setPressureCreator    $pressure_creator
$def_handler setTimeArrCreator     $time_arr_creator
$def_handler setTransducerCreator  $transducer_creator
$def_handler setTimeReference      $time_reference
$def_handler setRandomGenerator    $rand_generator
$def_handler setAltimetryCreator   $altimetry_creator ;#This line is needed for WOSS version newer than "woss-1.2.0"

WOSS/Utilities set debug 0
set woss_utilities [new "WOSS/Utilities"]

WOSS/Creator/Database/Binary/Results/TimeArr set debug           0
WOSS/Creator/Database/Binary/Results/TimeArr set woss_db_debug   0
WOSS/Creator/Database/Binary/Results/TimeArr set space_sampling 0.0

set db_res_arr [new "WOSS/Creator/Database/Binary/Results/TimeArr"]
set params(db_filename) "${params(db_res_path)}/my_sim_res_arr_no_db${params(numNodes)}_${params(bw)}.dat"

$db_res_arr setDbPathName $params(db_filename)

set time_evo_ssp_1 [new "WOSS/Definitions/Time"]
$time_evo_ssp_1 setTime 1 1 2011 9 0 1

WOSS/Database/Manager set debug 0
set db_manager [new WOSS/Database/Manager]
WOSS/Definitions/Altimetry/Flat set evolution_time_quantum   -1 ;#This line is needed for WOSS version newer than "woss-1.2.0"
WOSS/Definitions/Altimetry/Flat set range                    -1 ;#This line is needed for WOSS version newer than "woss-1.2.0"
WOSS/Definitions/Altimetry/Flat set total_range_steps        -1 ;#This line is needed for WOSS version newer than "woss-1.2.0"
WOSS/Definitions/Altimetry/Flat set depth                    0.0 ;#This line is needed for WOSS version newer than "woss-1.2.0"
set cust_altimetry   [new "WOSS/Definitions/Altimetry/Flat"] ;#This line is needed for WOSS version newer than "woss-1.2.0"
$db_manager setCustomSediment   "Test Sedim" 1560 200 1.5 0.9 0.8 1.0
$db_manager setCustomSSP    $time_evo_ssp_1    "./tcl_folder/ssp-test.txt"
$db_manager setCustomBathymetry $params(start_lat) $params(start_long) -500.0 4 0.0 100.0 500.0 200.0 1500.0 200.0 2500.0 100.0

WOSS/Creator/Bellhop set debug                        0
WOSS/Creator/Bellhop set woss_debug                   0
WOSS/Creator/Bellhop set max_time_values              10
WOSS/Creator/Bellhop set total_runs                   1
WOSS/Creator/Bellhop set frequency_step               0.0
WOSS/Creator/Bellhop set total_range_steps            10.0
WOSS/Creator/Bellhop set tx_min_depth_offset          0.0
WOSS/Creator/Bellhop set tx_max_depth_offset          0.0
WOSS/Creator/Bellhop set total_transmitters           1
WOSS/Creator/Bellhop set total_rx_depths              2
WOSS/Creator/Bellhop set rx_min_depth_offset          -0.1
WOSS/Creator/Bellhop set rx_max_depth_offset          0.1
WOSS/Creator/Bellhop set total_rx_ranges              2
WOSS/Creator/Bellhop set rx_min_range_offset          -0.1
WOSS/Creator/Bellhop set rx_max_range_offset          0.1
WOSS/Creator/Bellhop set total_rays                   0.0
WOSS/Creator/Bellhop set min_angle                    $params(node_min_angle)
WOSS/Creator/Bellhop set max_angle                    $params(node_max_angle)
WOSS/Creator/Bellhop set ssp_depth_precision          1.0e-8
WOSS/Creator/Bellhop set normalized_ssp_depth_steps   100000


set woss_creator [new "WOSS/Creator/Bellhop"]
$woss_creator setWorkDirPath        "./tmp/shm/woss/my_sim_no_db_${params(numNodes)}_${params(bw)}/"
$woss_creator setBellhopPath        ""
$woss_creator setBellhopMode        0 0 "A"
$woss_creator setBeamOptions        0 0 "B"
$woss_creator setBathymetryType     0 0 "L"
;#The following line is needed for WOSS version newer than "woss-1.2.0"
$woss_creator setAltimetryType      0 0 "L" ;#This line is needed for WOSS version newer than "woss-1.2.0"
$woss_creator setSimulationTimes    0 0 1 9 2010 0 0 1 2 9 2010 0 0 1


WOSS/Manager/Simple set debug 0
WOSS/Manager/Simple set space_sampling 0.0
set woss_manager [new "WOSS/Manager/Simple"]

WOSS/Definitions/TransducerHandler set debug 0
set transducer_handler [new WOSS/Definitions/TransducerHandler]


WOSS/Controller set debug 0
set woss_controller [new "WOSS/Controller"]
$woss_controller setTimeArrResultsDbCreator  $db_res_arr
$woss_controller setWossDbManager            $db_manager
$woss_controller setWossManager              $woss_manager
$woss_controller setWossCreator              $woss_creator
$woss_controller setTransducerhandler        $transducer_handler
$woss_controller initialize


WOSS/PlugIn/ChannelEstimator set debug_ 0.0

WOSS/ChannelEstimator set debug_           0.0
WOSS/ChannelEstimator set space_sampling_  0.0
WOSS/ChannelEstimator set avg_coeff_       0.5
set channel_estimator [ new "WOSS/ChannelEstimator"]


WOSS/Module/Channel set debug_                    0
WOSS/Module/Channel set channel_time_resolution_  -1.0

set channel [new "WOSS/Module/Channel"]
$channel setWossManager      $woss_manager
$channel setChannelEstimator $channel_estimator

WOSS/MPropagation set debug_ 0
set propagation [new "WOSS/MPropagation"]
$propagation setWossManager $woss_manager

set data_mask [new "MSpectralMask/Rect"]
$data_mask setFreq       $params(freq)
$data_mask setBandwidth  $params(bw)

WOSS/Module/MPhy/BPSK  set AcquisitionThreshold_dB_   $params(detectionDB)
WOSS/Module/MPhy/BPSK  set BitRate_                   $params(bitrate)
WOSS/Module/MPhy/BPSK  set MaxTxSPL_dB_               $params(txPower)
WOSS/Module/MPhy/BPSK  set MaxTxRange_                $params(txRadius)
WOSS/Module/MPhy/BPSK  set MinTxSPL_dB_               10
WOSS/Module/MPhy/BPSK  set MaxTxRange_                $params(txRadius)
WOSS/Module/MPhy/BPSK  set PER_target_                0.1
WOSS/Module/MPhy/BPSK  set TxSPLMargin_dB_            10
WOSS/Module/MPhy/BPSK  set RxSnrPenalty_dB_           -10.0
WOSS/Module/MPhy/BPSK  set SPLOptimization_           0
WOSS/Module/MPhy/BPSK  set CentralFreqOptimization_   0
WOSS/Module/MPhy/BPSK  set BandwidthOptimization_     0

##########################################################################################
