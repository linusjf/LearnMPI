#!/bin/bash

#Script Used for compiling and running openmpi programs
clear
source_file_name="./compare_bcast.c"
output_file_name="./compare.exe"
no_of_process=20

echo ---------Compiling Program------------

mpicc $source_file_name  -o $output_file_name -Wall
if [ $? == 0 ];then #No Errors Detected
	echo "COMPILE SUCCESS"
	echo ---------Executing Program------------
    termux-elf-cleaner $output_file_name &> /dev/null 
    mpirun --allow-run-as-root --hostfile ../hostfile -np $no_of_process --oversubscribe $output_file_name 100 100
else
	echo "COMPILE ERROR"
fi
#END OF SCRIPT
