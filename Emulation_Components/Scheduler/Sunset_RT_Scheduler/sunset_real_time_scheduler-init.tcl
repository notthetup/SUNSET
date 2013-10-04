# Dummy Initialization

Scheduler/Sunset_RealTime set adjust_new_width_interval_ 0;	# the interval (in unit of resize times) we recalculate bin width. 0 means disable dynamic adjustment
Scheduler/Sunset_RealTime set min_bin_width_ 1e-18;			# the lower bound for the bin_width
#Scheduler/Sunset_RealTime set maxslop_ 0.010; # max allowed slop b4 error (sec)
Scheduler/Sunset_RealTime set maxslop_ 1.0; # max allowed slop b4 error (sec)
Scheduler/Sunset_RealTime set simStartTime_ 0.0; # max allowed slop b4 error (sec)

