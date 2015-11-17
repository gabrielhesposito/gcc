#!/bin/bash
VAR=$(gcc database.c >> error.txt)
if ! [[ -z ${VAR+x} ]]; 
	then
		echo"No GCC errors found...continuing with valgrind"
		 valgrind ./a.out DB >> error.txt 	
	else
		echo"GCC errors found...Dont Worry! Check the Log!"
fi

