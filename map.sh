#!/bin/bash
IP=$(ifconfig br0 | grep 'inet addr' | cut -d: -f2 | awk '{print $1}')
RANGE=150-200
echo ${IP:0:-2}$RANGE
nmap ${IP:0:-2}$RANGE -n -sP | grep report | awk '{print $5}' >& active_machines.txt