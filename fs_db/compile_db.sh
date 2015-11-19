#!/bin/bash
VAR=$(gcc database.c);
x="hell bor";
if [ -z ${VAR+x} ]; 
	then
		echo "=============================================";
		echo "GCC errors found...Dont Worry! Check the Log!";
		echo "=============================================";
		echo "$VAR" >> "error.txt"; 	
	else
		echo "==============================================";
		echo "No GCC errors found...continuing with valgrind";
		echo "==============================================";
		valgrind ./a.out DB >> "error.txt" 2>&1;
fi

