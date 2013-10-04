if {$params(useStat) == 1} {
	
	global statistics

	set gen_packet [$statistics getGeneratedPacket]
	set pdr [$statistics getPDR]
	set delivered [$statistics getDeliveredPacket]
	set app_through [$statistics getApplicationThroughput]
	set latency [$statistics getPacketLatency]
	set route_len [$statistics getRouteLength]
	set num_route [$statistics getNumRoutes]
	set max_route_len [$statistics getMaxRouteLength]
	set over_bit [$statistics getOverheadPerBit]
	set mac_through [$statistics getMacThroughput]
	set mac_data_ret [$statistics getMacDataRetransmissions]
	set exp_time [$statistics getExperimentTime]
	set mac_data_disc [$statistics getMacDataPacketDiscarded]
	set mac_ctrl_disc [$statistics getMacCtrlPacketDiscarded]
	set resid_energy [$statistics getResidualEnergy]
	set idle_time [$statistics getIdleTime]
	set rx_time [$statistics getRxTime]
	set tx_tot_time [$statistics getTotTxTime]
	set idle_cons [$statistics getIdleConsumption]
	set rx_cons [$statistics getRxConsumption]
	set tx_tot_cons [$statistics getTotTxConsumption]
	set tx_time [$statistics getTxTime 180]
	set tx_cons [$statistics getTxConsumption 180]
	set tx_cons1 [$statistics getTxConsumption 2 180]

	puts "\n"
	puts "\n"
	puts "-1 -###################### STATISTICS ##################################"

	puts "-1 PDR 				$pdr"
	puts "-1 GeneratedPacket 		$gen_packet"
	puts "-1 DeliveredPacket 		$delivered"
	puts "-1 ApplicationThroughput 	$app_through"
	puts "-1 PacketLatency 		$latency"
	puts "-1 RouteLength 			$route_len"
	puts "-1 NumRoutes 			$num_route"
	puts "-1 MaxRouteLength 		$max_route_len"	
	puts "-1 OverheadPerBit 		$over_bit"
	puts "-1 MacThroughput 		$mac_through"
	puts "-1 MacDataRetransmissions 	$mac_data_ret"
	puts "-1 ExperimentTime 		$exp_time"
	puts "-1 MacDataPacketDiscarded 	$mac_data_disc"
	puts "-1 MacCtrlPacketDiscarded 	$mac_ctrl_disc"
	puts "-1 ResidualEnergy 		$resid_energy"
	puts "-1 IdleTime 			$idle_time"
	puts "-1 RxTime 			$idle_time"
	puts "-1 TotTxTime 			$tx_tot_time"
	puts "-1 IdleConsumption 		$idle_cons"
	puts "-1 RxConsumption 		$rx_cons"
	puts "-1 TotTxConsumption 		$tx_tot_cons"
	puts "-1 TotTxTime 			$tx_time"
	puts "-1 TxConsumption 		$tx_cons"

	puts "-1 -################### ------ALL INFO------ #############################"

}