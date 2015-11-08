#!/bin/bash
IP=$(ifconfig eth0 | grep 'inet addr' | cut -d: -f2 | awk '{print $1}')
##still needs to be dynamic no bother for dev
## find bottom of ip pool, first ip after router then sweep up
RANGE=150-200
nmap ${IP:0:-2}$RANGE -n -sP | grep report | awk '{print $5}' >& iplist.txt