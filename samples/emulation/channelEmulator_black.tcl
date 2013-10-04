# SUNSET - Sapienza University Networking framework for underwater Simulation, Emulation and real-life Testing
#
# Copyright (C) 2012 Regents of UWSN Group of SENSES Lab
#
# Author: Daniele Spaccini - spaccini@di.uniroma1.it
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
#	|          1.  Phy Layer	     | 
#	+------------------------------------+
#	| 	   Channel Emulator          |
#	+------------------------------------+
#
# Each node runs in emulation mode. More nodes can be defined specifying node IDs and sink ID. 
# Each node generates a CBR traffic creating one packet every cbr_period (set to 30) seconds. 

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
set params(start_traffic)		10.0
set params(end_traffic)			100.0
set params(seed)			0   			;#seed
set params(debug)			1			;#debug level, increasing the debug level will print out more information
set params(id)				1
set params(run_id)			1
set params(sink)			1
set params(randomDest) 			0
set params(numNodes)			7
set params(broadcast_address)   	-1
set params(pktDataSize) 		512
set params(max_pktDataSize) 		512

set params(genTraffic)			1
set params(propagationDelay)		1.5
set params(traffic_barrier)		10

#STAT_IFO
set params(useStat) 			0
set params(statFile)			""

set params(simulationMode) 		0
set params(urick)			0
set params(bellhop) 			0
set params(emulationMode)		1

#DEVICE DELAY
set params(device_delay)		0.1
set params(device_data_delay)		0.1
set params(device_ctrl_delay)		0.1

#MAC INFO
set params(headerSize)		3
set params(long_retry)		4
set params(short_retry)		7

#CHANNEL EMULATOR
set params(emulator_ip)		"127.0.0.1"
set params(emulator_port)	8000
set params(emulator_port_pos)	8001
set params(emulator_usePos)	1
set params(emulator_defProp)	1.0
set params(emulator_useNSPos)	1

#POSITION EMULATOR
set params(position_useNS) 	1
set params(position_tick)	0

#GENERIC MODEM
set params(genmodem_tcp)	1

#DEVICE DELAY
set params(device_delay)		0.1
set params(device_data_delay)		0.1
set params(device_ctrl_delay)		0.1

#MODEM DELAY
set params(mdm_delay)			0.1
set params(mdm_data_delay)		0.1
set params(mdm_ctrl_delay)		0.1

#GENERIC DRIVER DELAY

set params(bitrate)  	      	1000
set params(dataRate)		$params(bitrate);
set params(ctrlRate)		$params(bitrate);

#AGENT INFO
set params(lambda)			0  ;# poisson traffic (if 0 use cbr)
set params(usePktTime)			0  ;# use lambda generation packet per packet time
set params(cbr_period)			30 ;# cbr traffic (if 0 use lamba)
set params(seed)			1

############################################################

set usage "
options:
	-id 		<positive int --> node ID> 
	-sink 		<positive int --> sink ID>
	\[-cbr_period 	<positive value --> constant bir rate period>\]
	\[-debug  	<positive int --> debug level>\]"

for {set i 0} {$i < [llength $argv]} {incr i} {
    set arg [lindex $argv $i]
    if { ! [string compare $arg "--help" ] } {
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
load $pathSUNSET/libSunset_Core_PktConverter.so.0.0.0 
load $pathSUNSET/libSunset_Core_Ns_PktConverter.so.0.0.0 
load $pathSUNSET/libSunset_Core_Common_PktConverter.so.0.0.0 
load $pathSUNSET/libSunset_Core_Packet_Error_Model.so.0.0.0 
load $pathSUNSET/libSunset_Core_Energy_Model.so.0.0.0   

#EMULATION COMPONENTS-----------------------------

load $pathSUNSET/libSunset_Emulation_Real_Time_Scheduler.so.0.0.0 
load $pathSUNSET/libSunset_Emulation_Debug_Emulation.so.0.0.0 
load $pathSUNSET/libSunset_Emulation_Utilities_Emulation.so.0.0.0 
load $pathSUNSET/libSunset_Emulation_Connection.so.0.0.0 
load $pathSUNSET/libSunset_Emulation_Generic_Modem.so.0.0.0 
load $pathSUNSET/libSunset_Emulation_Timing_Emulation.so.0.0.0 
load $pathSUNSET/libSunset_Emulation_Channel_Emulator.so.0.0.0 
load $pathSUNSET/libSunset_Emulation_Position_Channel.so.0.0.0    

#NETWORK PROTOCOLS-----------------------------

load $pathSUNSET/libSunset_Networking_Agent.so.0.0.0     
load $pathSUNSET/libSunset_Networking_Mac.so.0.0.0       
load $pathSUNSET/libSunset_Networking_Phy.so.0.0.0       
load $pathSUNSET/libSunset_Networking_Routing.so.0.0.0  
load $pathSUNSET/libSunset_Networking_Transport.so.0.0.0         
load $pathSUNSET/libSunset_Networking_Aloha.so.0.0.0              
load $pathSUNSET/libSunset_Networking_Protocol_Statistics.so.0.0.0    
load $pathSUNSET/libSunset_Networking_Static_Routing.so.0.0.0  

load $pathSUNSET/libSunset_Networking_Agent_PktConverter.so.0.0.0 
load $pathSUNSET/libSunset_Networking_Mac_PktConverter.so.0.0.0 
load $pathSUNSET/libSunset_Networking_Routing_PktConverter.so.0.0.0 
load $pathSUNSET/libSunset_Networking_Protocol_Statistics.so.0.0.0     

puts "SUNSET libraries DONE"                                           

############################################################


set ns [new Simulator]

########### MODULEs SETTINGS  ##############################

set statisticsSim [new Sunset_Statistics]
$statisticsSim setUseStat 0

if { $params(simulationMode) == 1 } {
	puts "ERROR"
	exit 0;
} else {

	Sunset_Utilities_Emulation set experimentMode 0

	set utilityAddress [new Sunset_Address]
	$utilityAddress setBroadcastAddress	0

	set utilities [new Sunset_Utilities_Emulation]
	$utilities	setExperimentMode	0

	set debug [new Sunset_Debug_Emulation]
	$debug setDebug $params(debug)
	set traceModule [new Sunset_Trace]
}

proc begin-simulation { } {
	global params
 	remove-all-packet-headers
	add-packet-header Common IP LL SUNSET_MAC SUNSET_AGT MPhy SUNSET_RTG
}

########### MODULEs SETTINGS  ##############################

Module/Sunset_Static_Routing set debug_ false;

Module/MMac/Sunset_Aloha set debug_ false;
Module/MMac/Sunset_Aloha set MAC_HDR_SIZE $params(headerSize)

Module/Sunset_Agent set debug_ 		false;
Module/Sunset_Agent set DATA_SIZE  	$params(pktDataSize)
Module/Sunset_Agent set moduleAddress  -1

Scheduler/Sunset_RealTime set adjust_new_width_interval_ 0;	# the interval (in unit of resize times) we recalculate bin width. 0 means 	disable dynamic adjustment
Scheduler/Sunset_RealTime set min_bin_width_ 1e-18;			# the lower bound for the bin_width
Scheduler/Sunset_RealTime set maxslop_ 1.0; 	# max allowed slop b4 error (sec)

############################################################

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

if { $params(simulationMode) == 1 } {
	puts "Script designed to run in emulation mode, while simulation mode has been selected ERROR"
	exit 1
} 	 
############################################################

########### TIMING MODULE SETTINGS  ########################

Sunset_Timing_Emulation set deviceDelayCtrl_		$params(device_ctrl_delay)
Sunset_Timing_Emulation set deviceDelayData_		$params(device_data_delay)
Sunset_Timing_Emulation set deviceDelay_		$params(device_delay)

Sunset_Timing_Emulation set modemDelay_			$params(mdm_delay)
Sunset_Timing_Emulation set modemDelayCtrl_		$params(mdm_ctrl_delay)
Sunset_Timing_Emulation set modemDelayData_		$params(mdm_data_delay)

Sunset_Timing_Emulation set dataRate_			$params(bitrate)
Sunset_Timing_Emulation set ctrlRate_			$params(bitrate)

Sunset_Timing_Emulation set pDelay_			$params(propagationDelay)

Sunset_Timing_Emulation set sifs_			0.0 ;#	0.00010	
Sunset_Timing_Emulation set slotTime_			0.0 ;#  0.000020

############################################################

########### STARTING CHANNEL EMULATOR #########################

proc startChannel {} {

	global params channel
	
	set channel [new Module/Sunset_Channel_Emulator]
	
	$channel setChannelPort $params(emulator_port)
	$channel setPositionPort $params(emulator_port_pos)

	$channel positionEmulation $params(emulator_usePos)
	$channel setDefPropDelay	$params(emulator_defProp)
	$channel setModuleAddress $params(id)
	$channel start
}

proc startPosition {} {

	global params channel sunPos
	
	set sunPos [new Module/Sunset_Position_Channel]

	#position_tick --> how often read the node position

	$sunPos checkNSPos $params(position_useNS) $params(position_tick)
	$sunPos setModuleAddress	$params(id)
	$sunPos setIp			$params(emulator_ip)
	$sunPos setPort 		$params(emulator_port_pos)
}

############################################################

########### POSITION MODULE UTILITIES #########################

proc nodePosition {} {

	global position params

	source "./topology_channel_emu.tcl"

	puts "NODE($params(id)) POSITION: [$position($params(id)) getLatitude_] [$position($params(id)) getLongitude_] [$position($params(id)) getAltitude_]"
}


proc getSunsetLatitude {id} {
	
	global position params

	set latitude [$position($params(id)) getLatitude_]

	return $latitude
}

proc getSunsetLongitude {id} {

	global position params

	set longitude [$position($params(id)) getLongitude_]

	return $longitude
}

proc getSunsetDepth {id} {
	
	global position params

	set altitude [$position($params(id)) getAltitude_]

	return $altitude
}

############################################################

########### BLACKLISTING NODES #############################

proc createBlacklist {} {
	
	global params phy

	if { $params(id) == 1 } {
		$phy($params(id)) addToBlacklist 4
		$phy($params(id)) addToBlacklist 5
		$phy($params(id)) addToBlacklist 6
	}

	if { $params(id) == 2 } {
		$phy($params(id)) addToBlacklist 4
		$phy($params(id)) addToBlacklist 5
		$phy($params(id)) addToBlacklist 6
	}

	if { $params(id) == 3 } {
		$phy($params(id)) addToBlacklist 1
	}

	if { $params(id) == 4 } {
		$phy($params(id)) addToBlacklist 1
		$phy($params(id)) addToBlacklist 2
	}

	if { $params(id) == 5 } {
		$phy($params(id)) addToBlacklist 1
		$phy($params(id)) addToBlacklist 2
		$phy($params(id)) addToBlacklist 7
	}

	if { $params(id) == 6 } {
		$phy($params(id)) addToBlacklist 1
		$phy($params(id)) addToBlacklist 2
		$phy($params(id)) addToBlacklist 7
	}

	if { $params(id) == 7 } {
		$phy($params(id)) addToBlacklist 5
		$phy($params(id)) addToBlacklist 6
	}
}

############################################################

########### Packet CONVERTER SETTINGS  #####################
#
# Create the packet converter module and register all the 
# packet header layers used for packet conversion
#
############################################################
proc createPktConverter {} {

	global ns source_  params modem

	if { $params(emulationMode) == 1 } {

		Sunset_PktConverter set MAX_DATA_SIZE 	$params(max_pktDataSize)
		Sunset_PktConverter set ADDR_BITS          3
		Sunset_PktConverter set DATA_BITS          20
		Sunset_PktConverter set PKT_ID_BITS        14
		Sunset_PktConverter set TIME_BITS          24
		Sunset_PktConverter set TTL_BITS           4
		Sunset_PktConverter set TXTIME_BITS        24
		
		set pktConverter [new Sunset_PktConverter]
		set pktConverter_agt [new Sunset_PktConverter/Agent]
		set pktConverter_ns [new Sunset_PktConverter/Ns]

		set pktConverter_rtg [new Sunset_PktConverter/Routing]
		set pktConverter_mac [new Sunset_PktConverter/Mac]
   
		$pktConverter_agt useSource 1
		$pktConverter_agt useDest 1
		$pktConverter_agt usePktId 0
		$pktConverter_agt useData 1

		$pktConverter_mac useSource 1
		$pktConverter_mac useDest 1
		$pktConverter_mac usePktId 0

		$pktConverter_ns useTimestamp 1
		#$pktConverter_ns useIpDestPort 0
		#$pktConverter_ns useIpSourcePort 0
		#$pktConverter_ns useIpSource 0
		#$pktConverter_ns useIpDest 0
		$pktConverter_ns setPortBits 3
		$pktConverter_ns useNumHop 1
		$pktConverter_ns useTxTime 1
		#$pktConverter_ns usePktId 0
		#$pktConverter_ns useTTL 0
		#$pktConverter_ns usePrevHop 0
		#$pktConverter_ns useNextHop 0

		$pktConverter setMaxLevelId 3
		$pktConverter addPktConverter 3 $pktConverter_agt
		$pktConverter addPktConverter 2 $pktConverter_rtg
		$pktConverter addPktConverter 1 $pktConverter_mac

		$pktConverter addPktConverter 0 $pktConverter_ns

		$pktConverter_agt start	
		$pktConverter_rtg start
		$pktConverter_mac start
		
		$modem setPktConverter $pktConverter
	}	
}

############################################################

proc createNodeEmulation {id } {

	global modem phy ns tcl_folder node params routing_ mac cbr_ position source_

	set node($id) [$ns create-M_Node] 

	Module/Sunset_Agent set portNumber 0

	set source_($id) [new Module/Sunset_Agent] 
	set routing_($id) [new Module/Sunset_Static_Routing]
	set mac($id) [new Module/MMac/Sunset_Aloha]
	set phy($id) [new Module/MPhy/Sunset_Phy]

	Queue/Sunset_Queue set mean_pktsize_ $params(pktDataSize)      

	set queue($id) [new Queue/Sunset_Queue]
	set timing($id) [new Sunset_Timing_Emulation]

	set modem [new Sunset_Generic_Modem]

	set errors($id) [new Module/Sunset_Packet_Error_Model]

	$modem setModuleAddress $id
	$errors($id) errorModel "static" 0.1

	$timing($id) setModem $modem

	$source_($id) setModuleAddress $id $params(run_id)
	$source_($id) setDataSize $params(pktDataSize)
	$routing_($id) setModuleAddress $id

	$mac($id) setModuleAddress $id
	$queue($id) setModuleAddress $id

	$phy($id) setModuleAddress $id
	$phy($id) usePktErrorModel 1

	$mac($id) setQueue $queue($id)
	$mac($id) setTiming $timing($id)

	$modem useTcp $params(genmodem_tcp)

	if { $params(genmodem_tcp) == 1 } {

		$modem setIp $params(emulator_ip)
		$modem setPort $params(emulator_port)
		$modem setDataRate $params(bitrate)

	} else {
		puts "Serial Connection"
	}

	$phy($id) useChEmulator 1

	$node($id) addModule 5 $source_($id) 0 "SRC($id)"
	$node($id) addModule 4 $routing_($id) 0 "RTG($id)"
	$node($id) addModule 3 $mac($id) 0 "MAC($id)"
	$node($id) addModule 2 $phy($id) 0 "PHY($id)"
	$node($id) addModule 1 $modem 0 "CHA($id)"

	$node($id) setConnection $source_($id) $routing_($id) 1
	$node($id) setConnection $routing_($id) $mac($id) 1
	$node($id) setConnection $mac($id) $phy($id) 1
	$node($id) setConnection $phy($id) $modem 1

	set position($id) [new "Position/BM"]
	$node($id) addPosition $position($id)
	$position($id) setX_ 0.0
	$position($id) setY_ 0.0
	$position($id) setZ_ 0.0

	puts "Node($id) CREATED - EMULATION"
}

############# CONFIGURE INFORMATION DISPATCHER ###################
proc setDispatcher { id } {
	
	global info_dispatcher

	$info_dispatcher addParameter $id "MAC_RESET"
	$info_dispatcher addParameter $id "MAC_TX_DONE"
	$info_dispatcher addParameter $id "MAC_TX_ABORT"
	$info_dispatcher addParameter $id "MAC_TX_COMPLETED"

	$info_dispatcher addParameter $id "NODE_POSITION"
}
################################################################

##############################################################
# Transmit a packet from 'node' to 'dest' at  time
##############################################################

set rngTrafficStartTimes [new RNG]
set rngTrafficNode [new RNG]

$rngTrafficStartTimes seed [expr $params(id) * 0.1] 
$rngTrafficNode seed [expr $params(id) * 0.1] 

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

proc genTrafficEmulation {} {

	global ns opt source_ time TRAFFIC_RATE params  nodeList  rngTrafficStartTimes rngTrafficNode 
	
	set nowT [$ns now]
	set time $nowT

	if { $params(genTraffic) == 0 } {
		return 
	}

	puts "in genTrafficEmulation time $nowT time $time paramsTime $params(cbr_period)"

	set node $params(id)
	set sinkNode [expr $params(sink)]

	puts "node $node $params(id) $params(id) sink $sinkNode - Traffic $TRAFFIC_RATE"

	while 1 {
		if { $params(randomDest) == 1 } {

			set numnodi [expr $params(numNodes) + 1 ]
			set rdnnode [$rngTrafficNode uniform 1 $numnodi]

			if { $numnodi > $params(numNodes) } {

				set numnodi $params(numNodes)
			}

			set sinkNode [expr round(floor($rdnnode))]

			if { $sinkNode == $params(id) } continue
		}

		if { $TRAFFIC_RATE != -1.0 } {

			if { $node == $sinkNode } {

				puts "I am the sink"
				break
			}

			set x [$rngTrafficStartTimes uniform 0 1]
			set deltatime [expr - 1 / $TRAFFIC_RATE * log($x)]
			set old_time $time

			if { $deltatime < 1.0 } {

				set deltatime [expr $deltatime + 1]
			}

			puts "($nowT) node $node time $time deltaTime $deltatime"
		
			set nowT [$ns now]
			set time [expr $nowT + $deltatime]
			puts "($nowT) Create source $node  at time $time"

		} else {

			if { $params(cbr_period) == 0 } {

				puts "($nowT) node $node return genTrafficEmulation"
				return
			}

			set nodeSend $node
			set nowT [$ns now]

			set time [expr $nowT + $params(cbr_period)]
		}

		if { $params(end_traffic) != -1 } {
			if { $time > $params(end_traffic) - $params(traffic_barrier)} break
		}

		puts "Node $params(id) at $time genTrafficEmulation paramTime $params(cbr_period)"
		if { $sinkNode != $node } {

			make_source_nodeDest $node [expr $nowT + 1 ] $sinkNode
		}
		break
	}
	if { $params(end_traffic) == -1 } {

		if { $node != $sinkNode } {

			$ns at $time "genTrafficEmulation"    
		}

	} else {
		if { $time <= $params(end_traffic) -  $params(traffic_barrier)} {

			if { $node != $sinkNode } {
				$ns at $time "genTrafficEmulation"    
			}
		}
	}
}

proc startModule {} {

	puts "START MODULE ENTER"
	global utilities ns phy modem params time  source_  routing_ mac statistics info_dispatcher sunPos

	## ADD STATIC ROUTES FOR STATIC ROUTING ##
	$routing_($params(id)) add_route 0 0
	$routing_($params(id)) add_route 1 1
	$routing_($params(id)) add_route 2 2
	$routing_($params(id)) add_route 3 3
	$routing_($params(id)) add_route 4 4
	$routing_($params(id)) add_route 5 5 
	$routing_($params(id)) add_route 6 6
	$routing_($params(id)) add_route 7 7
	$routing_($params(id)) add_route 8 8
	$routing_($params(id)) add_route 9 9
	$routing_($params(id)) add_route 10 10
	$routing_($params(id)) add_route 11 11
	$routing_($params(id)) add_route 12 12
	$routing_($params(id)) add_route 13 13
	$routing_($params(id)) add_route 14 14
	$routing_($params(id)) add_route 15 15
	###########################################

	$utilities start

	$info_dispatcher start
	$source_($params(id)) start
	$routing_($params(id)) start
	$mac($params(id)) start
	$phy($params(id)) start
	$modem start
	$sunPos start

	puts "MODULE STARTED"	
}

proc endModule {} {

	global utilities ns phy params time START_DELAY  source_ routing_ mac statistics info_dispatcher 

	$utilities stop

	$info_dispatcher stop
	$source_($params(id)) stop
	$routing_($params(id)) stop
	$mac($params(id)) stop
}

proc finish {} {

	global ns params statistics cbr_sink_ cbr_ source_ channel
	global modem sunPos

	$modem stop
	$sunPos stop
	$channel stop

	$ns flush-trace
	close $params(tracefile)
	$ns halt

	if {$params(useStat) == 1} {
		$statistics stop
	}

	set txPkt 0
	set rxPkt 0
	set latency 0.0
	set countDelay 0
	set res1 0 
	set res2 0
	set res3 0

	for {set id2 0} {$id2 < $params(numNodes)} {incr id2}  {			
		set res1 [ $source_($params(id)) getTxPkt $id2 ]
		set res2 [ $source_($params(id)) getRxPkt $id2 ]
		set res3 [ $source_($params(id)) getDelay $id2 ]
		set rxPkt [ expr $rxPkt + $res2]
		set latency [ expr $latency + (double($res3)) ]
		if { (double($res3)) > 0.0} {
			set countDelay [expr $countDelay + 1 ]
		}
		set txPkt [ expr $txPkt + $res1]
	}

	puts "\n"
	puts "\n"
	puts "-1 -###################### START ID  ##################################"
	puts "-1 -txPkt 			= $txPkt"
	puts "-1 -rxPkt 			= $rxPkt"
	if { $txPkt == 0 } {
		puts "-1 -Throughput 			= 0"
	} else {
		puts "-1 -Throughput 			= [expr (double($rxPkt)) / (double($txPkt))]"
	}
	puts "-1 -NodeSending 		= $countDelay"
	if { $rxPkt == 0 } {
		puts "-1 -AverageLatency 		= 0.0"
	} else {
		puts "-1 -AverageLatency 		= [expr (double($latency)) / (double($rxPkt))]"
	}
	puts "-1 -###################### ------ALL INFO------ ##################################"
}

proc startTraffic {} {

	global ns  params START_DELAY  source_ userModule   routing_ mac statistics info_dispatcher 

	set nowT [$ns now]
    	set time $nowT
	
	if { $params(genTraffic) == 1 } {
		if { $time <= $params(start_traffic) } {
			$ns at $params(start_traffic) "genTrafficEmulation"
			puts "First packet generated at [expr $params(start_traffic) + $params(cbr_period)]"
		} else {
			genTrafficEmulation
			puts "First packet generated at [expr $nowT + $params(cbr_period)]"
		}
	}
}

$ns use-Miracle
$ns use-scheduler Sunset_RealTime
$ns trace-all $params(tracefile)

begin-simulation

Module/Sunset_Information_Dispatcher set debug_ false
set info_dispatcher [new Module/Sunset_Information_Dispatcher]

setDispatcher $params(id)

startChannel
createNodeEmulation $params(id)
nodePosition
startPosition
createPktConverter
createBlacklist

if { $params(emulationMode) == 1 } {

	$debug start
}

set nowT [$ns now]
set startModuleTime [expr $nowT + 5]

$ns at $startModuleTime "startModule"
$ns at $params(start_traffic) "startTraffic"
$ns at [expr $params(end_traffic) + 10.0]  "endModule"
$ns at [expr $params(end_traffic) + 13.0]  "finish"

$ns run


