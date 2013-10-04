# SUNSET - Sapienza University Networking framework for underwater Simulation, Emulation and real-life Testing
#
# Copyright (C) 2012 Regents of UWSN Group of SENSES Lab
#
# Author: Roberto Petroccia - petroccia@di.uniroma1.it
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License as published
# at http://creativecommons.org/licenses/by-nc-sa/3.0/legalcode
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANATBILITY or FITNESS FOR A PARTICULAR PURPOSE. See the Creative Commons
# Attribution-NonCommercial-ShareAlike 3.0 Unported License for more details.
#
# You should have received a copy of the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
# along with this program. If not, see <http://creativecommons.org/licenses/by-nc-sa/3.0/legalcode>.
#
#
#
# Node architecture
#
#	+------------------------------------+
#	|          4. Agent Layer            | 
#	+------------------------------------+
#	|  3. Routing Layer (Static routing) | 
#	+------------------------------------+
#	|      2. Mac Layer (Basic Aloha)    | 
#	+------------------------------------+
#	|  1. BPSK Phy Layer (SUNSET Urick)  | 
#	+------------------------------------+
#	| 	  Channel: Urick module      |
#	+------------------------------------+
#
# On average one packet is generated in the network every 400 seconds. 
# Every time a new packet has to be generated, one node in the network, 
# with the exception of the sink, is randomly selected as data source.
#

########### PARAMETERS INIZIALIZATION ######################
global def_rng
set def_rng [new RNG]
$def_rng default

#TRACE INFO
set params(tracefilename) 	"/dev/null"
set params(tracefile) 		[open $params(tracefilename) w]
set params(cltracefilename) 	"/dev/null"
set params(cltracefile) 	[open $params(cltracefilename) w]

#SIM INFO
set params(start)			0.0
set params(start_traffic)		100.0
set params(end_traffic)			10000.0
set params(seed)			0   			;#seed
set params(debug)			0			;#debug level, increasing the debug level will print out more information
set params(id)				-1
set params(sink)			1
set params(broadcast_address)   	0
set params(start_lat)     		42.32
set params(start_long)    		10.22
set params(sink_depth)          	10
set params(pktDataSize) 		512
set params(numNodes)			5
set params(traffic_barrier)		1000.0
set params(randomDest)			0 ;# use a random destination (sink param ignored)	
set params(txRadius)			1000
set params(rep_num)		     	10
set params(run_id)			1 ;# run id
set params(genTraffic) 			1

#CHANNEL INFO
set params(txPower)     	      	180
set params(node_min_angle)	       -89
set params(node_max_angle)      	89
set params(propagationDelay)		1.5
set params(freq)           		25000
set params(bw)             		5000	;# 5kHz

set params(bitrate)  	      		5000	;# 5000 bps
set params(dataRate)			$params(bitrate);
set params(ctrlRate)			$params(bitrate);
set params(baudRate)			19200

set params(detectionDB)			1
set params(maxinterval_)		500.0
set params(wind)			7.0
set params(ship)			0.5

#STAT_IFO
set params(useStat) 			0
set params(statFile)			""

#ENERGY INFO

set params(maxEnergy)			4040000
set params(txCons)			3
set params(rxCons)			0.85
set params(idleCons)			0.085

for {set k 0} {$k < $params(rep_num)} {incr k} {
     $def_rng next-substream
}

set params(simulationMode) 		1
set params(urick)			1
set params(bellhop) 			0
set params(emulationMode)		0

#MAC INFO
set params(headerSize)			3
set params(long_retry)			4
set params(short_retry)			7

#AGENT INFO
set params(lambda)			0  ;# poisson traffic (if 0 use cbr)
set params(usePktTime)			0  ;# use lambda generation packet per packet time
set params(cbr_period)			400 ;# cbr traffic (if 0 use lamba)
set params(seed)			1

########### PARSING PARAMETERS  ##############################

for {set i 0} {$i < [llength $argv]} {incr i} {
    set arg [lindex $argv $i]
    if { ! [string compare $arg "-help" ] } {
	puts $usage
	exit 1
    }
    set key [string range $arg 1 end]
    if { [catch "set dummy $params($key)"] } {
	puts "Unknown option $arg"
	puts "\n$usage"
	exit 1
    } else {
	incr i
	set params($key) [lindex $argv $i]
    }
}

############################################################

########### LOAD LIBRARIES  ##############################

puts "Loading Miracle libraries"

set pathMiracle "insert_miracle_libraries_path_here"

if { $pathMiracle == "insert_miracle_libraries_path_here" } {
  puts "You have to set the Miracle libraries path first."
  exit
}

load $pathMiracle/libMiracle.so.0.0.0
load $pathMiracle/libmiraclecbr.so.0.0.0
load $pathMiracle/libMiracleWirelessCh.so.0.0.0
load $pathMiracle/libmphy.so.0.0.0
load $pathMiracle/libMiracleBasicMovement.so.0.0.0
load $pathMiracle/libmmac.so.0.0.0
load $pathMiracle/libMiracleIp.so.0.0.0
load $pathMiracle/libmiracletcp.so.0.0.0
load $pathMiracle/libMiraclePhy802_11.so.0.0.0
load $pathMiracle/libMiracleMac802_11.so.0.0.0
load $pathMiracle/libmiracleport.so.0.0.0
load $pathMiracle/libmll.so.0.0.0
load $pathMiracle/libmiraclelink.so.0.0.0
load $pathMiracle/libMiracleRouting.so.0.0.0
load $pathMiracle/libMiracleAodv.so.0.0.0
load $pathMiracle/libcbrtracer.so.0.0.0
load $pathMiracle/libsinrtracer.so.0.0.0
load $pathMiracle/libmphymaccltracer.so.0.0.0
load $pathMiracle/libverboseclcmntracer.so.0.0.0                                                         
load $pathMiracle/libMiracleIp.so.0.0.0
load $pathMiracle/libMiracleIpRouting.so.0.0.0
load $pathMiracle/libmiracleport.so.0.0.0

puts "Miracle libraries DONE"

#-----------------------------

puts "Loading WOSS libraries"
set pathWOSS "insert_woss_libraries_path_here"

if { $pathWOSS == "insert_woss_libraries_path_here" } {
  puts "You have to set the WOSS libraries path first."
  exit
}
load $pathWOSS/libUwmStd.so.0.0.0
load $pathWOSS/libWOSS.so.0.0.0
load $pathWOSS/libWOSSPhy.so.0.0.0
load $pathWOSS/libUwmStdPhyBpskTracer.so.0.0.0

puts "WOSS libraries DONE"     

#----------------------------------------------

puts "Loading SUNSET libraries"

set pathSUNSET "insert_sunset_libraries_path_here"

if { $pathSUNSET == "insert_sunset_libraries_path_here" } {
  puts "You have to set the SUNSET libraries path first."
  exit
}

#CORE COMPONENTS-----------------------------
                                                                 
load $pathSUNSET/libSunset_Core_Utilities.so.0.0.0 
load $pathSUNSET/libSunset_Core_Information_Dispatcher.so.0.0.0       
load $pathSUNSET/libSunset_Core_Module.so.0.0.0       
load $pathSUNSET/libSunset_Core_Common_Header.so.0.0.0       
load $pathSUNSET/libSunset_Core_Statistics.so.0.0.0       
load $pathSUNSET/libSunset_Core_Timing.so.0.0.0       
load $pathSUNSET/libSunset_Core_Queue.so.0.0.0     
load $pathSUNSET/libSunset_Core_Phy_Mac.so.0.0.0       
load $pathSUNSET/libSunset_Core_Mac_Routing.so.0.0.0       
load $pathSUNSET/libSunset_Core_Modem_Phy.so.0.0.0 
load $pathSUNSET/libSunset_Core_Packet_Error_Model.so.0.0.0 
load $pathSUNSET/libSunset_Core_Energy_Model.so.0.0.0   

#NETWORK PROTOCOLS-----------------------------

load $pathSUNSET/libSunset_Networking_Agent.so.0.0.0     
load $pathSUNSET/libSunset_Networking_Mac.so.0.0.0       
load $pathSUNSET/libSunset_Networking_Phy.so.0.0.0       
load $pathSUNSET/libSunset_Networking_Routing.so.0.0.0  
load $pathSUNSET/libSunset_Networking_Transport.so.0.0.0         
load $pathSUNSET/libSunset_Networking_Aloha.so.0.0.0              
load $pathSUNSET/libSunset_Networking_Protocol_Statistics.so.0.0.0    
load $pathSUNSET/libSunset_Networking_Phy_Urick.so.0.0.0  
load $pathSUNSET/libSunset_Networking_Static_Routing.so.0.0.0      

puts "SUNSET libraries DONE"                                           

############################################################

set phyPreambleTime 10.0 ;# we assume a preamble of 10 ms at th ephysical layer for training and signal detection which is kept into account when transmitting a packet in water

set phyHeader [ expr (double($phyPreambleTime)/1000.0) * (double($params(dataRate)))]
set phyHeader [ expr ceil($phyHeader /8.0) ]

########### MODULEs SETTINGS  ##############################
if { $params(simulationMode) == 1 } {
	Sunset_Utilities set experimentMode 1	;# 1 = SIMULATION MODE - 0 = EMULATION MODE
} else {
	Sunset_Utilities set experimentMode 0	;# 1 = SIMULATION MODE - 0 = EMULATION MODE
}

Module/Sunset_Static_Routing set debug_ false;

Module/MMac/Sunset_Mac set debug_ false;
Module/MMac/Sunset_Mac set MAC_HDR_SIZE	[expr $params(headerSize) + $phyHeader]

Module/MMac/Sunset_Aloha set debug_ false;
Module/MMac/Sunset_Aloha set MAC_HDR_SIZE	[expr $params(headerSize) + $phyHeader]

Module/MPhy/Sunset_Phy set debug_ false;

Module/Sunset_Agent set debug_ 		false;
Module/Sunset_Agent set DATA_SIZE  	$params(pktDataSize)
Module/Sunset_Agent set moduleAddress  -1

Queue/Sunset_Queue set		mean_pktsize_	$params(pktDataSize)      

############################################################

########### TIMING MODULE SETTINGS  ########################

Sunset_Timing set dataRate_			$params(dataRate)
Sunset_Timing set ctrlRate_			$params(ctrlRate)
Sunset_Timing set baudRate_			$params(baudRate)

Sunset_Timing set pDelay_			$params(propagationDelay)

Sunset_Timing set sifs_				0.00010	
Sunset_Timing set slotTime_			0.000020
############################################################


proc begin-simulation { } {
	global params
 	remove-all-packet-headers
	add-packet-header Common IP LL SUNSET_MAC SUNSET_AGT MPhy SUNSET_RTG
}

########### PARAMETERS CHECKING  ########################
set sum [ expr $params(simulationMode) + $params(emulationMode) ]

if { $sum == 0 } {
	puts "No simulation or emulation modes have been set ERROR"
	exit 1
}  

if { $sum > 1 } {
	puts "Both simulation and emulation modes have been set ERROR"
	exit 1
}  

if { $params(emulationMode) == 1 } {
	puts "Script designed to run in simulation mode, while emulation mode has been selected ERROR"
	exit 1
} 	 

if { $params(simulationMode) == 1 } {
	set sum [ expr $params(urick) + $params(bellhop) ]

	if { $sum == 0 } {
		puts "No urick or bellhop models have been set ERROR"
		exit 1
	}  

	if { $sum > 1 } {
		puts "Both urick and bellhop models have been set ERROR"
		exit 1
	}
} 	 

if { $params(bellhop) == 1 } {
	puts "Script designed to run in simulation mode using Urick channel model, while bellhop model has been selected ERROR"
	exit 1
} 	 
############################################################

Module/Sunset_Information_Dispatcher set debug_ false
set info_dispatcher [new Module/Sunset_Information_Dispatcher]

##################################
# Configure information dispatcher
##################################
$info_dispatcher addParameter $params(id) "MAC_RESET"
$info_dispatcher addParameter $params(id) "MAC_TX_DONE"
$info_dispatcher addParameter $params(id) "MAC_TX_ABORT"
$info_dispatcher addParameter $params(id) "MAC_TX_COMPLETED"


############################################################

##################################
# Load Debug module
##################################
set debug [new Sunset_Debug]
set traceModule [new Sunset_Trace]
$debug setDebug	$params(debug)

############################################################

set startTime 5.0

set ns [new Simulator]
$ns use-Miracle

if {$params(urick) == 1} {
	source "./tcl_folder/SUNSETUrickFile.tcl"
}

##################################
# Configure Utilities
##################################
Sunset_Utilities set experimentMode 0
set utilities [new Sunset_Utilities]
$utilities	setExperimentMode	1

set utilityAddress [new Sunset_Address]
$utilityAddress setBroadcastAddress $params(broadcast_address)

proc createNode { id }  {
	global channel propagation data_mask ns  position_ node_ portnum_ energy
	global phy params mac_ woss_utilities source_  routing_ cbr_

	set node_($id) [$ns create-M_Node $params(tracefile) $params(cltracefile)] 
  
	set source_($id) 	[new "Module/Sunset_Agent"] 
	set routing_($id) 	[new "Module/Sunset_Static_Routing"]
	set mac_($id) 		[new "Module/MMac/Sunset_Aloha"]
	set phy($id) 		[new "Module/Sunset_Phy_Urick"]

	set queue($id) 		[new "Queue/Sunset_Queue"]
	set timing($id) 	[new "Sunset_Timing"]
	set energy($id) 	[new "Module/Sunset_Energy_Model"]

	set errors($id) [new Module/Sunset_Packet_Error_Model]
	
	$source_($id) setModuleAddress $id
	$routing_($id) setModuleAddress $id
	$mac_($id) setModuleAddress $id
	$queue($id) setModuleAddress $id

	$source_($id) setDataSize $params(pktDataSize)

	$mac_($id) setQueue $queue($id)
	$mac_($id) setTiming $timing($id)

	$energy($id) setInitialEnergy	$params(maxEnergy)

	$energy($id) setTxPower 	$params(txPower) $params(txCons)
	$energy($id) setRxPower	$params(rxCons)
	$energy($id) setIdlePower	$params(idleCons)
	$energy($id) setModuleAddress  	$id

	$errors($id) errorModel "fsk" -10.0

	$phy($id) addEnergyModule $energy($id)
	$phy($id) setModuleAddress $id
	$phy($id) addPower $params(txPower)
	$phy($id) usePktErrorModel 1

	$node_($id) addModule 4 $source_($id) 0 "SRC($id)"
	$node_($id) addModule 3 $routing_($id) 0 "RTG($id)"
	$node_($id) addModule 2 $mac_($id) 0 "MAC($id)"
	$node_($id) addModule 1 $phy($id) 0 "PHY($id)"

	$node_($id) setConnection $source_($id) $routing_($id) 1
	$node_($id) setConnection $routing_($id) $mac_($id) 1
	$node_($id) setConnection $mac_($id) $phy($id) 1
	$node_($id) addToChannel $channel $phy($id)   0

	set position_($id) [new "WOSS/Position/WayPoint"]
	$node_($id) addPosition $position_($id)
	set posdb($id) [new "PlugIn/PositionDB"]
	$node_($id) addPlugin $posdb($id) 20 "PDB"
	$posdb($id) addpos $id $position_($id)

	set interf_data($id) [new "MInterference/MIV"]
	$interf_data($id) set maxinterval_ $params(maxinterval_)
	$interf_data($id) set debug_       0

	$phy($id) setSpectralMask       $data_mask
	$phy($id) setPropagation        $propagation
	$phy($id) setInterference       $interf_data($id)

	$position_($id) setLatitude_ 0
	$position_($id) setLongitude_ 0
	$position_($id) setAltitude_  0  

     puts "NODE($id) CREATED"
}

proc createSink { } {
	global channel propagation data_mask ns  position_ node_ portnum_ energy
	global phy params mac_ woss_utilities source_  routing_ cbr_sink_ portnum_sink_

	set node_($params(sink)) [$ns create-M_Node $params(tracefile) $params(cltracefile)] 

	set source_($params(sink)) 	[new "Module/Sunset_Agent"] 
	set routing_($params(sink)) 	[new "Module/Sunset_Static_Routing"]
	set mac_($params(sink)) 	[new "Module/MMac/Sunset_Aloha"]
	set phy($params(sink)) 		[new "Module/Sunset_Phy_Urick"]

	set queue_($params(sink)) 	[new "Queue/Sunset_Queue"]
	set timing_($params(sink))	[new "Sunset_Timing"]
	set energy($params(sink)) 	[new "Module/Sunset_Energy_Model"]

	set errors($params(sink)) [new Module/Sunset_Packet_Error_Model]
	
	$source_($params(sink)) setModuleAddress $params(sink)
	$routing_($params(sink)) setModuleAddress $params(sink)
	$queue_($params(sink)) setModuleAddress $params(sink)
	$mac_($params(sink)) setModuleAddress $params(sink)

	$source_($params(sink)) setDataSize $params(pktDataSize)

	$mac_($params(sink)) setQueue $queue_($params(sink))
	$mac_($params(sink)) setTiming $timing_($params(sink))

	$energy($params(sink)) setInitialEnergy	$params(maxEnergy)

	$energy($params(sink)) setTxPower 	$params(txPower) $params(txCons)
	$energy($params(sink)) setRxPower	$params(rxCons)
	$energy($params(sink)) setIdlePower	$params(idleCons)
	$energy($params(sink)) setModuleAddress  	$params(sink)

	$errors($params(sink)) errorModel "fsk" -10.0

	$phy($params(sink)) addEnergyModule $energy($params(sink))
	$phy($params(sink)) setModuleAddress $params(sink)
	$phy($params(sink)) addPower $params(txPower)
	$phy($params(sink)) usePktErrorModel 1

	$node_($params(sink)) addModule 4 $source_($params(sink)) 0 "SRC($params(sink))"
	$node_($params(sink)) addModule 3 $routing_($params(sink)) 0 "RTG($params(sink))"
	$node_($params(sink)) addModule 2 $mac_($params(sink)) 0 "MAC($params(sink))"
	$node_($params(sink)) addModule 1 $phy($params(sink)) 0 "PHY($params(sink))"


	$node_($params(sink)) setConnection $source_($params(sink)) $routing_($params(sink)) 1
	$node_($params(sink)) setConnection $routing_($params(sink)) $mac_($params(sink)) 1
	$node_($params(sink)) setConnection $mac_($params(sink)) $phy($params(sink)) 1
	$node_($params(sink)) addToChannel $channel $phy($params(sink))   0

	set position_($params(sink)) [new "WOSS/Position/WayPoint"]
	$node_($params(sink)) addPosition $position_($params(sink))
	set posdb($params(sink)) [new "PlugIn/PositionDB"]
	$node_($params(sink)) addPlugin $posdb($params(sink)) 20 "PDB"
	$posdb($params(sink)) addpos $params(sink) $position_($params(sink))

	set interf_data($params(sink)) [new "MInterference/MIV"]
	$interf_data($params(sink)) set maxinterval_ $params(maxinterval_)
	$interf_data($params(sink)) set debug_       0

	$phy($params(sink)) setSpectralMask       $data_mask
	$phy($params(sink)) setPropagation        $propagation
	$phy($params(sink)) setInterference       $interf_data($params(sink))

	$position_($params(sink)) setLatitude_ 0
	$position_($params(sink)) setLongitude_ 0
	$position_($params(sink)) setAltitude_  0  
	
	puts "SINK CREATED"
}


###############################
# Load node positions
###############################
proc createPosition { id  }  {

	global position_ woss_utilities source_ params coord_x

	source "topology_sim.tcl"
}

############################################################


##############################################################
# Transmit a packet from 'node' to 'dest' at  time
##############################################################

set rngTrafficStartTimes [new RNG]
set rngTrafficNode [new RNG]

$rngTrafficStartTimes seed 1; #2000 ;#$params(seed)
$rngTrafficNode seed $params(seed); #$params(seed)

set TRAFFIC_RATE       -1.0

proc sendDataSimDest {node dest} {
	global ns node_  timeClock timeaux source_
	$source_($node) send $dest
}

proc make_source_nodeDest {node start dest} {

	global ns 

	set nowT [$ns now]

	if { $start > [expr $nowT ] } {

		$ns at $start "sendDataSimDest $node $dest" 
	} else {

		puts "NO TRANSMISSION OCCURS"	
	}
}



if { $params(lambda) == 1 } {
	
	if { $params(usePktTime) == 1 } {
		set pktTime [ expr  (double($params(pktDataSize) * 8.0)) / (double($params(dataRate))) ]
	} else {
		set pktTime 1
	}
	
	set TRAFFIC_RATE        [expr $pktTime / (double($params(cbr_period)))]
	set TRAFFIC_RATE        [expr 1.0 / (double($TRAFFIC_RATE))]

	puts "Start Test!!! pktTime $pktTime - TRAFFIC_RATE $TRAFFIC_RATE"
}

proc genTrafficSimulation {} {

	global ns source_ time TRAFFIC_RATE  params global_count nodeList rngTrafficStartTimes rngTrafficNode 
	
	set nowT [$ns now]
	set time $nowT
	
	puts "in genTrafficSimulation time $nowT time $time"
	
	if { $params(genTraffic) == 0 } {
			
		return 
	}

	set sinkNode [expr $params(sink)]

	puts "sink $sinkNode - Traffic $TRAFFIC_RATE - BARRIER $params(traffic_barrier)"
	
	while 1 {
		set numnodi [expr $params(numNodes)+1]

		if { $params(randomDest) == 1 } {
			set rdnnode [$rngTrafficNode uniform 1 $numnodi]
			set sinkNode [expr round(floor($rdnnode))]
		}

		set rdnnode [$rngTrafficNode uniform 1 $numnodi]
		set node [expr round(floor($rdnnode))]

		if { $node == $sinkNode || $node >= $params(numNodes)+1 } continue

		if { $TRAFFIC_RATE != -1.0 } {

			set x [$rngTrafficStartTimes uniform 0 1]
			set deltatime [expr - 1 / $TRAFFIC_RATE * log($x)]
			set old_time $time

			set nowT [$ns now]
			set time [expr $nowT + $deltatime]

		} else {

			set time [expr $time + $params(cbr_period)]
		}

		if { $time > $params(end_traffic) -  $params(traffic_barrier)} {

			puts "EXIT GENERATE TRAFFIC time $time - end $params(end_traffic) $params(traffic_barrier)" 
			break
		}

		puts "Node $node at $time generate_traffic paramTime $params(cbr_period) to $sinkNode"

		make_source_nodeDest $node $time $sinkNode

		break
	}
	if { $time <= $params(end_traffic) -  $params(traffic_barrier)} {

		if { $node != $sinkNode } {
			$ns at $time "genTrafficSimulation"    
		}
	}
}
############################################################


$ns trace-all $params(tracefile)

set nowT [$ns now]

set startModuleTime [expr $nowT + 5]
$ns at $startModuleTime "startModule"

proc startModule { } {

	global ns  params time START_DELAY  source_  routing_ mac_ statistics info_dispatcher energy phy

	## ADD STATIC ROUTES FOR STATIC ROUTING ##
	for {set id 1} {$id <= $params(numNodes)} {incr id}  {
		for {set id2 1} {$id2 <= $params(numNodes)} {incr id2}  {
			$routing_($id) add_route $id2 $id2 	;# node add_route destination relay (we assume single-hop network in this example)
		}
	}
	###########################################

	if {$params(useStat) == 1} {
	     $statistics start
	}

	$info_dispatcher start

	for {set id 1} {$id <= $params(numNodes)} {incr id}  {
		$source_($id) start
		$routing_($id) start
		$mac_($id) start
		$energy($id) start
		$phy($id) start
	}
}

proc endModule { } {

	global params source_ statistics mac_ routing_ energy phy info_dispatcher

	$info_dispatcher stop

	for {set id 1} {$id <= $params(numNodes)} {incr id}  {
		$source_($id) stop
		$routing_($id) stop
		$mac_($id) stop
		$energy($id) stop
		$phy($id) stop
	}
}

proc finish {} {

	global ns params db_manager statistics cbr_sink_ cbr_ energy source_

	if { $params(bellhop) == 1 } {
		$db_manager closeAllConnections
	}

	$ns flush-trace
	close $params(tracefile)
	$ns halt

	if {$params(useStat) == 1} {

		source "tcl_folder/printStat.tcl"

	} else {

		set txPkt 0
		set rxPkt 0
		set latency 0.0
		set countDelay 0
		set hop 0

		for {set id 1} {$id <= $params(numNodes)} {incr id}  {

			set res1 0 
			set res2 0
			set res3 0
			set res4 0

			for {set id2 1} {$id2 <= $params(numNodes)} {incr id2}  {			
				set res1 [ $source_($id) getTxPkt $id2 ]
				set res2 [ $source_($id) getRxPkt $id2 ]
				set res3 [ $source_($id) getDelay $id2 ]
				set res4 [ $source_($id) getPktHop $id2 ]

				set rxPkt [ expr $rxPkt + $res2]
				set latency [ expr $latency + (double($res3)) ]
				set hop [ expr $hop + (double($res4)) ]
				if { (double($res3)) > 0.0} {
					set countDelay [expr $countDelay + 1 ]
				}
				set txPkt [ expr $txPkt + $res1]
			}
		}
		puts "\n"
		puts "\n"
		puts "-1 -###################### START ID  ##################################"
		puts "-1 -txPkt 			= $txPkt"
		puts "-1 -rxPkt 			= $rxPkt"
		if { $txPkt == 0 } {
			puts "-1 -PDR 			= 0"
		} else {
			puts "-1 -PDR 			= [expr (double($rxPkt)) / (double($txPkt))]"
		}
		# 	puts "-1 -latency 			= $latency"
		puts "-1 -NodeSending 		= $countDelay"
		if { $rxPkt == 0 } {
			puts "-1 -AverageLatency 		= 0.0"
			puts "-1 -AverageHop 			= 0.0"
		} else {
			puts "-1 -AverageLatency 		= [expr (double($latency)) / (double($rxPkt))]"
			puts "-1 -AverageHop 			= [expr (double($hop)) / (double($rxPkt))]"
		}

		for {set id 1} {$id <= $params(numNodes)} {incr id}  {
			puts "($id) -Residual energy          = [$energy($id) getResidualEnergy]"
			puts "($id) -Time spent in idle          = [$energy($id) getIdleTime]"
			puts "($id) -Energy consumed in idle     = [$energy($id) getIdleConsumption]"
			puts "($id) -Time spent in rx            = [$energy($id) getRxTime]"
			puts "($id) -Energy consumed in rx       = [$energy($id) getRxConsumption]"
			puts "($id) -Time spent in tx            = [$energy($id) getTotTxTime]"
			puts "($id) -Energy consumed in tx         = [$energy($id) getTotTxConsumption]"
		}

		puts "-1 -###################### ------ALL INFO------ ##################################"
	}
}
 

###############################
# Load packet headers
###############################
begin-simulation

set tcl_precision 6

############################################################

###############################
# create nodes
###############################
for {set id 1} {$id <= $params(numNodes)} {incr id}  {
	if { $params(sink) != $id } {
		createNode $id
	} else {
		createSink
	}
}

############################################################

###############################
# create node positions
###############################
for {set id 1} {$id <= $params(numNodes)} {incr id}  {
	createPosition $id
}


############################################################

###################
# Loading Statistics
###################

if { $params(useStat) == 1 } {

	Sunset_Protocol_Statistics set binaryOutput_ 0

	set statistics [new Sunset_Protocol_Statistics]

	$statistics setUseStat $params(useStat)
	$statistics setPhyPreambleSize $phyHeader
	$statistics setStartTraffic $params(start_traffic)
	$statistics setRunId $params(run_id)
	$statistics setMaxNodeId $params(numNodes)
	$statistics setTotalEnergy $params(maxEnergy)

	set aux ""
	append aux "statistics_" $params(run_id) ".txt"
	puts $aux
	$statistics setOutputFile $aux
	$statistics setLogFile "LogFile_$params(run_id).txt"
}

############################################################

###################
# start traffic
###################

proc startTraffic {} {
    global ns  params START_DELAY  source_ userModule   routing_ mac statistics info_dispatcher 
	
	set nowT [$ns now]
	set time $nowT

	genTrafficSimulation
}


############################################################

###################
# start simulation
###################

puts "\nSimulating...\n"

$ns at $params(start_traffic) "startTraffic"
$ns at [expr $params(end_traffic) + 1000.0]  "endModule"
$ns at [expr $params(end_traffic) + 1003.0]  "finish"

puts "Start Test!!!"

$ns run

