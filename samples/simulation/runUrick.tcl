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
#	|  	       6. CBR                |
#	+------------------------------------+
#	| 	       5. Port               |
#	+------------------------------------+
#	| 	     4. Transport            |
#	+------------------------------------+
#	|  3. Routing Layer (Static routing) | 
#	+------------------------------------+
#	|      2. Mac Layer (Basic Aloha)    | 
#	+------------------------------------+
#	|      1. BPSK Phy Layer (Urick)     | 
#	+------------------------------------+
#	| 	  Channel: Urick module      |
#	+------------------------------------+
#
# Five Nodes are in the network. Sink is node 1, other nodes generate CBR traffic to the sink.
# Each node generates on average one packet every 400 seconds.
#
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
set params(txRadius)			1000
set params(rep_num)		     	10

set params(lambda)			0

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

#CBR INFO
set params(cbr_period)			400
set params(lambda)			0

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

Module/CBR set packetSize_          $params(pktDataSize)

if { $params(lambda) == 1 } {
	Module/CBR set period_              [expr 1.0 / (double($params(cbr_period)))]
	Module/CBR set PoissonTraffic_      1		;# poisson traffic
} else {
	Module/CBR set period_              $params(cbr_period)
	Module/CBR set PoissonTraffic_      0		;# CBR traffic
}

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
	source "./tcl_folder/UrickFile.tcl"
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
	global channel propagation data_mask ns  position_ node_ portnum_
	global phy params mac_ woss_utilities source_  routing_ cbr_

	set node_($id) [$ns create-M_Node $params(tracefile) $params(cltracefile)] 
  
	set cbr_($id)       	[new "Module/CBR"] 
	set port_($id)      	[new "Module/Port/Map"]
	set transport_($id) 	[new "Module/Sunset_Transport"] 
	set routing_($id) 	[new "Module/Sunset_Static_Routing"]
	set mac_($id) 		[new "Module/MMac/Sunset_Aloha"]
	set phy($id) 		[new "Module/MPhy/BPSK/Underwater"]

	set queue($id) 		[new "Queue/Sunset_Queue"]
	set timing($id) 	[new "Sunset_Timing"]
	
	$transport_($id) setModuleAddress $id
	$routing_($id) setModuleAddress $id
	$mac_($id) setModuleAddress $id
	$queue($id) setModuleAddress $id

	$mac_($id) setQueue $queue($id)
	$mac_($id) setTiming $timing($id)

	$node_($id) addModule 6 $cbr_($id)       0 "CBR($id)"
	$node_($id) addModule 5 $port_($id)      0 "PRT($id)"
	$node_($id) addModule 4 $transport_($id) 0 "TRA($id)"
	$node_($id) addModule 3 $routing_($id) 0 "RTG($id)"
	$node_($id) addModule 2 $mac_($id) 0 "MAC($id)"
	$node_($id) addModule 1 $phy($id) 0 "PHY($id)"

	$node_($id) setConnection $cbr_($id) $port_($id) 1
	$node_($id) setConnection $port_($id) $transport_($id) 1
	$node_($id) setConnection $transport_($id) $routing_($id) 1
	$node_($id) setConnection $routing_($id) $mac_($id) 1
	$node_($id) setConnection $mac_($id) $phy($id) 1
	$node_($id) addToChannel $channel $phy($id)   0

	set portnum_($id) [$port_($id) assignPort $cbr_($id)]

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

	for { set id 1} {$id <= $params(numNodes)} {incr id} {
		set cbr_sink_($id)       [new "Module/CBR"] 
	}
	set port_($params(sink))      	[new "Module/Port/Map"]
	set transport_($params(sink)) 	[new "Module/Sunset_Transport"] 
	set routing_($params(sink)) 	[new "Module/Sunset_Static_Routing"]
	set mac_($params(sink)) 	[new "Module/MMac/Sunset_Aloha"]
	set phy($params(sink)) 		[new "Module/MPhy/BPSK/Underwater"]

	set queue_($params(sink)) 	[new "Queue/Sunset_Queue"]
	set timing_($params(sink))	[new "Sunset_Timing"]
	
	$transport_($params(sink)) setModuleAddress $params(sink)
	$routing_($params(sink)) setModuleAddress $params(sink)
	$queue_($params(sink)) setModuleAddress $params(sink)

	$mac_($params(sink)) setModuleAddress $params(sink)
	$mac_($params(sink)) setQueue $queue_($params(sink))
	$mac_($params(sink)) setTiming $timing_($params(sink))

	for { set id 1} {$id <= $params(numNodes)} {incr id} {
		$node_($params(sink)) addModule 6 $cbr_sink_($id) 0 "CBR_sink"
	}     

	$node_($params(sink)) addModule 5 $port_($params(sink))      0 "PRT($params(sink))"
	$node_($params(sink)) addModule 4 $transport_($params(sink)) 0 "TRA($params(sink))"
	$node_($params(sink)) addModule 3 $routing_($params(sink)) 0 "RTG($params(sink))"
	$node_($params(sink)) addModule 2 $mac_($params(sink)) 0 "MAC($params(sink))"
	$node_($params(sink)) addModule 1 $phy($params(sink)) 0 "PHY($params(sink))"

	for { set id 1} {$id <= $params(numNodes)} {incr id} {
		$node_($params(sink)) setConnection $cbr_sink_($id)  $port_($params(sink))     1
	}

	$node_($params(sink)) setConnection $port_($params(sink)) $transport_($params(sink)) 1
	$node_($params(sink)) setConnection $transport_($params(sink)) $routing_($params(sink)) 1
	$node_($params(sink)) setConnection $routing_($params(sink)) $mac_($params(sink)) 1
	$node_($params(sink)) setConnection $mac_($params(sink)) $phy($params(sink)) 1
	$node_($params(sink)) addToChannel $channel $phy($params(sink))   0

	for { set id 1} {$id <= $params(numNodes)} {incr id} {
		set portnum_sink_($id) [$port_($params(sink)) assignPort $cbr_sink_($id)]
	}

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

$ns trace-all $params(tracefile)

set nowT [$ns now]

set startModuleTime [expr $nowT + 5]
$ns at $startModuleTime "startModule"

proc startModule { } {

	global ns  params time START_DELAY  source_  routing_ mac_ statistics info_dispatcher energy phy

	## ADD STATIC ROUTES FOR STATIC ROUTING ##
	for {set id 1} {$id <= $params(numNodes)} {incr id}  {
		for {set id2 0} {$id2 <= $params(numNodes)} {incr id2}  {
			$routing_($id) add_route $id2 $id2 	;# node add_route destination relay (we assume single-hop network in this example)
		}
	}
	###########################################

	if {$params(useStat) == 1} {
	     $statistics start
	}

	$info_dispatcher start

	for {set id 1} {$id <= $params(numNodes)} {incr id}  {
		$routing_($id) start
		$mac_($id) start
	}
}

proc endModule { } {

	global params source_ statistics mac_ routing_ energy phy

	for {set id 1} {$id <= $params(numNodes)} {incr id}  {
		$routing_($id) stop
		$mac_($id) stop
	}
}

proc finish {} {

	global ns params db_manager statistics cbr_sink_ cbr_ energy

	if { $params(bellhop) == 1 } {
		$db_manager closeAllConnections
	}

	$ns flush-trace
	close $params(tracefile)
	$ns halt

	if {$params(useStat) == 1} {
		$statistics stop
	}

	puts "\n"
	puts "\n"

	for {set id 1} {$id <= $params(numNodes)} {incr id}  {

		set cbr_throughput   [$cbr_sink_($id) getthr]
		set cbr_delay        [$cbr_sink_($id) getftt]
		set cbr_per          [$cbr_sink_($id) getper]
		set cbr_pkts         0

		if { $params(sink) != $id } {
			set cbr_pkts [$cbr_($id) getsentpkts]
		}

		set cbr_rxpkts       [$cbr_sink_($id) getrecvpkts]

		puts "($id)      	app data pkts created       : $cbr_pkts"
		puts "($id) 		app data pkts received      : $cbr_rxpkts"
		puts "($id) 		throughput                  : $cbr_throughput"
		puts "($id) 		normalized throughput       : [expr $cbr_throughput / (($params(pktDataSize) * 8.0) / $params(cbr_period))]"
		puts "($id) 		delay                       : $cbr_delay"
		puts "($id) 		packet error rate           : $cbr_per"
		puts ""

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

###############################
# create CBR connections
###############################
proc connectNodes { id } {
	global cbr_ params  portnum_sink_ portnum_ cbr_sink_
	$cbr_($id) set destAddr_ $params(sink)
	$cbr_($id) set destPort_ $portnum_sink_($id)

	$cbr_sink_($id) set destAddr_ $id
	$cbr_sink_($id) set destPort_ $portnum_($id) 
}

for {set id 1} {$id <= $params(numNodes)} {incr id}  {
	if { $params(sink) != $id } {
	    connectNodes $id
	}
}

############################################################

###################
# start CBR(s)
###################
for {set id 1} {$id <= $params(numNodes)} {incr id}  {

	set traffic_delay 0
	if { $params(lambda) == 0 } {
		set traffic_delay [expr $id * 5] ;# this is to avoid that all nodes start generating data packet at the same time
	}

	if { $params(sink) != $id } {
    		$ns at [expr $params(start_traffic)+$traffic_delay]   "$cbr_($id) start" 
		$ns at $params(end_traffic)    "$cbr_($id) stop"
	}
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
# start simulation
###################

puts "\nSimulating...\n"

$ns at [expr $params(end_traffic) + 1000.0]  "endModule"
$ns at [expr $params(end_traffic) + 1003.0]  "finish"

puts "Start Test!!!"

$ns run

