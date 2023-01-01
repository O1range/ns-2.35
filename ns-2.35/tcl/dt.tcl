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
# $ns color 0 blue
# $ns color 1 red
# $ns color 2 white
#create .tr file
set f [open dt.tr w]
$ns trace-all $f
# set nf [open out_lh.nam w]
# $ns namtrace-all $nf
#create nodes lh
# set sendno1 415
# set sendno2 415
set sendno 140
set recvno 2
set switchno 1
set send_long_num 10
set send_short_num [expr $sendno-$send_long_num]
set queue_num 2
#switchno,0
for {set i 0} {$i < $switchno} {incr i} {
    set sw($i) [$ns node]
}
#sendno,1---140
for {set i 0} {$i < $sendno} {incr i} {
    set se($i) [$ns node]
}

#recvno,141,142
for {set i 0} {$i < $recvno} {incr i} {
    set re($i) [$ns node]
}

#send---switch
for {set i 0} {$i < $sendno} {incr i} {
    $ns duplex-link $se($i) $sw(0) 100Gb 2us DropTail
}

#switch----recv
$ns duplex-link $sw(0) $re(0) 100Gb 2us DTQueue
$ns duplex-link $sw(0) $re(1) 100Gb 2us DTQueue

#lh TCP Agent
for {set i 0} {$i < $sendno} {incr i} {
    set tcp($i) [new Agent/TCP/Newreno]
    $ns attach-agent $se($i) $tcp($i)
    $tcp($i) set flowid_ $i
}

#lh TCP sink
#0--69--->re(0)
for {set i 0} {$i < 70} {incr i} {
    set sink($i) [new Agent/TCPSink]
    $ns attach-agent $re(0) $sink($i)
    $ns connect $tcp($i) $sink($i)
}
#70--139--->re(1)
for {set i 70} {$i < 140} {incr i} {
    set sink($i) [new Agent/TCPSink]
    $ns attach-agent $re(1) $sink($i)
    $ns connect $tcp($i) $sink($i)
}

#lh application
#0---9
for {set i 0} {$i < 10} {incr i} {
    set ftp($i) [new Application/FTP]
    $ftp($i) attach-agent $tcp($i)
    $ns at 0.2 "$ftp($i) send [expr 1460*2000]"
    # $ns at [expr 0.2+$i*0.0000001] "$ftp($i) send [expr 1460*6000]"
}
#70---79
for {set i 70} {$i < 80} {incr i} {
    set ftp($i) [new Application/FTP]
    $ftp($i) attach-agent $tcp($i)
    $ns at 0.2 "$ftp($i) send [expr 1460*2000]"
    # $ns at [expr 0.2+$i*0.0000001] "$ftp($i) send [expr 1460*6000]"
}
#10---69
for {set i 10} {$i < 70} {incr i} {
    set ftp($i) [new Application/FTP]
    $ftp($i) attach-agent $tcp($i)
}

for {set i 0} {$i < 3} {incr i} {
    set asy 0
    for {set j [expr 10+20*$i]} {$j < 30+20*$i } {incr j} {
        
        $ns at [expr 0.2010+$i*0.00000002+$asy*0.000000001] "$ftp($j) send [expr 1460*20]"
        # $ns at [expr 0.2010+$i*0.000045+$asy*0.000000002] "$ftp($j) send [expr 1460*20]"
        incr asy
    }
}

#80---139
for {set i 80} {$i < 140} {incr i} {
    set ftp($i) [new Application/FTP]
    $ftp($i) attach-agent $tcp($i)
}

for {set i 0} {$i < 3} {incr i} {
    set asy 0
    for {set j [expr 80+20*$i]} {$j < 100+20*$i } {incr j} {
        
        $ns at [expr 0.2010+$i*0.00000002+$asy*0.000000001] "$ftp($j) send [expr 1460*20]"
        # $ns at [expr 0.2010+$i*0.00001+$asy*0.000000002] "$ftp($j) send [expr 1460*20]"
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

