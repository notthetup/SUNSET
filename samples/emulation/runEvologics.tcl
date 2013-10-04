# SUNSET - Sapienza University Networking framework for underwater Simulation, Emulation and real-life Testing
#
# Copyright (C) 2011 Regents of UWSN Group of SENSES Lab
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
#	|          6. CBR Layer              | 
#	+------------------------------------+
#	|          5. Port Layer             | 
#	+------------------------------------+
#	|          4. Tranport Layer         | 
#	+------------------------------------+
#	|  3. Routing Layer (Static routing) | 
#	+------------------------------------+
#	|      2. Mac Layer (Basic Aloha)    | 
#	+------------------------------------+
#	|          1.  Phy Layer	     | 
#	+------------------------------------+
#	| 	   Evologics Modem 	     |
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
set params(sink)			0
set params(numNodes)			7
set params(broadcast_address)   	-1
set params(pktDataSize) 		64
set params(max_pktDataSize) 		64

set params(propagationDelay)		1.5

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

#EVOLOGICS
set params(evo)			1
set params(evoPort)		9200		;# it defines the modem port
set params(socketEvoId)		"127.0.0.1"	;# it defines the modem IP
set params(evoPower) 		3		;# it defines the modem transmission power
set params(evo_bitrate)		500
set params(evo_data_bitrate)	500
set params(evo_ctrl_bitrate)	500
set params(evo_max_pkt_size)	$params(max_pktDataSize)
set params(evo_version)		"1.6"

#MODEM DELAY
set params(evo_delay)		0.3
set params(evo_data_delay)	0.3
set params(evo_ctrl_delay)	0.3

set params(bitrate)  	      	$params(evo_bitrate)
set params(dataRate)		$params(bitrate);
set params(ctrlRate)		$params(bitrate);

set params(cbr_period)			30
set params(lambda)			0

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

if { $params(evo_version) == "1.4" } {
	load $pathSUNSET/libSunset_Emulation_Evologics_v_one_four.so.0.0.0 
} else {
	load $pathSUNSET/libSunset_Emulation_Evologics_v_one_six.so.0.0.0 
}

#NETWORK PROTOCOLS-----------------------------

load $pathSUNSET/libSunset_Networking_Agent.so.0.0.0     
load $pathSUNSET/libSunset_Networking_Mac.so.0.0.0       
load $pathSUNSET/libSunset_Networking_Phy.so.0.0.0       
load $pathSUNSET/libSunset_Networking_Routing.so.0.0.0  
load $pathSUNSET/libSunset_Networking_Transport.so.0.0.0         
load $pathSUNSET/libSunset_Networking_Aloha.so.0.0.0              
load $pathSUNSET/libSunset_Networking_Protocol_Statistics.so.0.0.0    
load $pathSUNSET/libSunset_Networking_Static_Routing.so.0.0.0  

load $pathSUNSET/libSunset_Networking_Cbr_PktConverter.so.0.0.0 
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
	add-packet-header Common IP LL SUNSET_MAC CBR MPhy SUNSET_RTG
}
########### MODULEs SETTINGS  ##############################

Module/Sunset_Agent set debug_ 		false;
Module/Sunset_Agent set DATA_SIZE  	$params(pktDataSize)
Module/Sunset_Agent set moduleAddress  -1

Module/Sunset_Static_Routing set debug_ false;

Module/MMac/Sunset_Aloha set debug_ false;
Module/MMac/Sunset_Aloha set MAC_HDR_SIZE $params(headerSize)

Module/CBR set packetSize_          $params(pktDataSize)

if { $params(lambda) == 1 } {
	Module/CBR set period_              [expr 1.0 / (double($params(cbr_period)))]
	Module/CBR set PoissonTraffic_      1		;# poisson traffic
} else {
	Module/CBR set period_              $params(cbr_period)
	Module/CBR set PoissonTraffic_      0		;# CBR traffic
}

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

Sunset_Timing_Emulation set modemDelay_			$params(evo_delay)
Sunset_Timing_Emulation set modemDelayCtrl_		$params(evo_ctrl_delay)
Sunset_Timing_Emulation set modemDelayData_		$params(evo_data_delay)

Sunset_Timing_Emulation set dataRate_			$params(evo_data_bitrate)
Sunset_Timing_Emulation set ctrlRate_			$params(evo_ctrl_bitrate)
Sunset_Timing_Emulation set baudRate_			19200

Sunset_Timing_Emulation set pDelay_			$params(propagationDelay)

Sunset_Timing_Emulation set sifs_			0.0 ;#	0.00010	
Sunset_Timing_Emulation set slotTime_			0.0 ;#  0.000020

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
		Sunset_PktConverter set DATA_BITS          8
		Sunset_PktConverter set PKT_ID_BITS        14
		Sunset_PktConverter set TIME_BITS          24
		Sunset_PktConverter set TTL_BITS           4
		
		set pktConverter [new Sunset_PktConverter]
		set pktConverter_cbr [new Sunset_PktConverter/Cbr]		
		set pktConverter_ns [new Sunset_PktConverter/Ns]

		set pktConverter_rtg [new Sunset_PktConverter/Routing]
		set pktConverter_mac [new Sunset_PktConverter/Mac]

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
		#$pktConverter_ns usePktId 0
		#$pktConverter_ns useTTL 0
		#$pktConverter_ns usePrevHop 0
		#$pktConverter_ns useNextHop 0

		$pktConverter setMaxLevelId 3
		$pktConverter addPktConverter 3 $pktConverter_cbr		
		$pktConverter addPktConverter 2 $pktConverter_rtg
		$pktConverter addPktConverter 1 $pktConverter_mac

		$pktConverter addPktConverter 0 $pktConverter_ns

		$pktConverter_cbr start	
		$pktConverter_rtg start
		$pktConverter_mac start
		
		$modem setPktConverter $pktConverter
	}	
}

############################################################

proc createNodeEmulation {id } {

	global modem phy ns tcl_folder node params routing_ mac cbr_ cbr_sink_ portnum_ port_

	set node($id) [$ns create-M_Node] 

	Module/Sunset_Agent set portNumber 0

	set cbr_($id)       	[new "Module/CBR"] 
	set port_($id)      	[new "Module/Port/Map"]
	set transport_($id) 	[new "Module/Sunset_Transport"] 
	set routing_($id)	[new "Module/Sunset_Static_Routing"]
	set mac($id) 		[new "Module/MMac/Sunset_Aloha"]
	set phy($id) 		[new "Module/MPhy/Sunset_Phy"]

	Queue/Sunset_Queue set mean_pktsize_ $params(pktDataSize)      

	set queue($id) [new Queue/Sunset_Queue]
	set timing($id) [new Sunset_Timing_Emulation]

	if { $params(evo_version) == "1.4" } {
		Sunset_Evologics_v1.4 set is_ranging 1
		set modem [new Sunset_Evologics_v1.4]
	} else {
		Sunset_Evologics_v1.6 set is_ranging 1
		set modem [new Sunset_Evologics_v1.6]
	}

	$modem setModuleAddress $id

	$timing($id) setModem $modem

	$routing_($id) setModuleAddress $id

	$mac($id) setModuleAddress $id
	$queue($id) setModuleAddress $id

	$phy($id) setModuleAddress $id

	$mac($id) setQueue $queue($id)
	$mac($id) setTiming $timing($id)

	if { $params(socketEvoId) == "." } {
		puts "The IP address of the evologics modem has not been defined ERROR, please set the params(socketEvoId) value"
		exit 0
	}

	$modem setModuleAddress $id
	$modem setConnectionType "tcp" $params(socketEvoId) $params(evoPort) 
	$modem setTxPower 		$params(evoPower)

	$modem setDataRate $params(bitrate)

	$node($id) addModule 7 $cbr_($id)       0 "CBR($id)"
	$node($id) addModule 6 $port_($id)      0 "PRT($id)"
	$node($id) addModule 5 $transport_($id) 0 "TRA($id)"
	$node($id) addModule 4 $routing_($id) 0 "RTG($id)"
	$node($id) addModule 3 $mac($id) 0 "MAC($id)"
	$node($id) addModule 2 $phy($id) 0 "PHY($id)"
	$node($id) addModule 1 $modem 0 "CHA($id)"

	$node($id) setConnection $cbr_($id) $port_($id) 1
	$node($id) setConnection $port_($id) $transport_($id) 1
	$node($id) setConnection $transport_($id) $routing_($id) 1
	$node($id) setConnection $routing_($id) $mac($id) 1
	$node($id) setConnection $mac($id) $phy($id) 1
	$node($id) setConnection $phy($id) $modem 1

	set portnum_($id) [$port_($id) assignPort $cbr_($id)]

	$timing($id) setModem $modem

	puts "Node($id) CREATED - EMULATION"
}

############# CONFIGURE INFORMATION DISPATCHER ###################
proc setDispatcher { id } {
	
	global info_dispatcher

	$info_dispatcher addParameter $id "MAC_RESET"
	$info_dispatcher addParameter $id "MAC_TX_DONE"
	$info_dispatcher addParameter $id "MAC_TX_ABORT"
	$info_dispatcher addParameter $id "MAC_TX_COMPLETED"

	$info_dispatcher addParameter $id "NODE_PROPAGATION_DELAY"
	$info_dispatcher addParameter $id "TX_POWER"
}
################################################################

proc startModule {} {

	puts "START MODULE ENTER"
	global utilities ns phy modem params time  source_  routing_ mac statistics info_dispatcher

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
	$routing_($params(id)) start
	$mac($params(id)) start
	$phy($params(id)) start
	$modem start

	puts "MODULE STARTED"	
}

proc endModule {} {

	global utilities ns phy params time START_DELAY  source_ routing_ mac statistics info_dispatcher 

	$utilities stop

	$info_dispatcher stop
	$routing_($params(id)) stop
	$mac($params(id)) stop
}

proc finish {} {

	global ns params statistics cbr_sink_ cbr_

	$ns flush-trace
	close $params(tracefile)
	$ns halt

	if {$params(useStat) == 1} {
		$statistics stop
	}
}

if { $params(emulationMode) == 1 } {

	$debug start
}

$ns use-Miracle
$ns use-scheduler Sunset_RealTime
$ns trace-all $params(tracefile)

begin-simulation

Module/Sunset_Information_Dispatcher set debug_ false
set info_dispatcher [new Module/Sunset_Information_Dispatcher]

setDispatcher $params(id)

createNodeEmulation $params(id)
createPktConverter
 
###############################
# create CBR connections
###############################
proc connectNodes { } {
	global cbr_ params  portnum_sink_ portnum_ cbr_sink_

	$cbr_($params(id)) set destAddr_ $params(sink)
	$cbr_($params(id)) set destPort_ 1

	if { $params(id) == $params(sink) } {

		$cbr_($params(sink)) set destAddr_ 1
		$cbr_($params(sink)) set destPort_ 1
	}
}

############################################################

###################
# start CBR(s)
###################
if { $params(sink) != $params(id) } {

	set traffic_delay 0
	if { $params(lambda) == 0 } {
		set traffic_delay [expr $params(id) * 3] ;# this is to avoid that multiple instances of SUNSET running on the same machine and started at the same time will always generate data at the same exact time
	}

	$ns at [expr $params(start_traffic)+$traffic_delay]   "$cbr_($params(id)) start"
	$ns at $params(end_traffic)    "$cbr_($params(id)) stop"
}
############################################################

connectNodes 

set nowT [$ns now]
set startModuleTime [expr $nowT + 5]

$ns at $startModuleTime "startModule"
$ns at [expr $params(end_traffic) + 10.0]  "endModule"
$ns at [expr $params(end_traffic) + 13.0]  "finish"

$ns run


