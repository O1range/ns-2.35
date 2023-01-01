#
#400 senders send short flow in 20 turns
#in one turn 20 senders send short flow at the different time
#
#
#
set ns [new Simulator]
Agent/TCP set packetSize_ 1460
Application/Traffic/CBR set packetSize_ 1460
Agent/TCP set minrto_ 0.2 ; # minRTO = 200ms
Agent/TCP/Newreno set minrto_ 0.00002 
Agent/TCP set maxrto_ 0.2
Agent/TCP/Newreno set maxrto_ 0.00002
Agent/TCP set slow_start_restart_ false
Agent/TCP/Newreno set slow_start_restart_ false
Agent/TCP/FullTcp set interval_ 0.04
Agent/TCP/Newreno set interval_ 0.04
DelayLink set avoidReordering_ true
 Agent/TCP set tcpTick_ 0.00001
 Agent/TCP/Newreno set tcpTick_ 0.00001
$ns color 0 blue
$ns color 1 red
$ns color 2 white
#create .tr file
set f [open out_lh2.tr w]
$ns trace-all $f
# set nf [open out_lh.nam w]
# $ns namtrace-all $nf
#create nodes lh
set sendno 415
set recvno 1
set switchno 1
set send_long_num 15
set send_short_num [expr $sendno-$send_long_num]
#sendno
for {set i 0} {$i < $sendno} {incr i} {
    set se($i) [$ns node]
}
#recvno
for {set i 0} {$i < $recvno} {incr i} {
    set re($i) [$ns node]
}
#switchno
for {set i 0} {$i < $switchno} {incr i} {
    set sw($i) [$ns node]
}
#send---switch
for {set i 0} {$i < $sendno} {incr i} {
    $ns duplex-link $se($i) $sw(0) 100Gb 2us DropTail
}
#switch----recv
$ns duplex-link $sw(0) $re(0) 100Gb 2us lhqueue

#lh TCP Agent
for {set i 0} {$i < $sendno} {incr i} {
    set tcp($i) [new Agent/TCP/Newreno]
    $ns attach-agent $se($i) $tcp($i)
    $tcp($i) set flowid_ $i
}

#lh TCP sink
for {set i 0} {$i < $sendno} {incr i} {
    set sink($i) [new Agent/TCPSink]
    $ns attach-agent $re(0) $sink($i)
    $ns connect $tcp($i) $sink($i)
}

#lh application
for {set i 0} {$i < $send_long_num} {incr i} {
    set ftp($i) [new Application/FTP]
    $ftp($i) attach-agent $tcp($i)
    $ns at 0.2 "$ftp($i) send [expr 1460*10]"
    # $ns at [expr 0.2+$i*0.0000001] "$ftp($i) send [expr 1460*6000]"
}
for {set i $send_long_num} {$i < $sendno} {incr i} {
    set ftp($i) [new Application/FTP]
    $ftp($i) attach-agent $tcp($i)
}
# set turn 10
# for {set j 0} {$j < 20} {incr j} {
#     for {set i $send_long_num} {$i < $sendno} {incr i} {
#         $ns at [expr 0.201+$j*0.001] "$ftp($i) send [expr 1460*10]"
#     }
# }
for {set i 0} {$i < 3} {incr i} {
    set asy 0
    for {set j [expr 15+20*$i]} {$j < 35+20*$i } {incr j} {
        
        # $ns at [expr 0.2010+$i*0.001+$asy*0.000000002] "$ftp($j) send [expr 1460*20]"
        $ns at [expr 0.2010+$i*0.00002+$asy*0.000000002] "$ftp($j) send [expr 1460*20]"
        incr asy
    }
}


$ns at 3.0 "finish"

proc finish {} {
	global ns f 
	$ns flush-trace
	close $f
	

	puts "running nam..."
	
	exit 0
}

$ns run

