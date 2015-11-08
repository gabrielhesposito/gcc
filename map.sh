#!/bin/bash

nmap $1 -n -sP | grep report | awk '{print $5}' >& active_machines.txt