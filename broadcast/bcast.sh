#!/bin/bash

#Script Used for compiling and running openmpi programs
clear
source_file_name="./bcast.cpp"
output_file_name="./bcast.exe"
no_of_process=3

echo ---------Compiling Program------------

mpic++ $source_file_name -g -o $output_file_name -Wall
if [ $? == 0 ];then #No Errors Detected
	echo "COMPILE SUCCESS"
	echo ---------Executing Program------------
    termux-elf-cleaner $output_file_name &> /dev/null 
    mpirun --allow-run-as-root --hostfile ../hostfile -np $no_of_process $output_file_name < nums.txt
else
	echo "COMPILE ERROR"
fi
#END OF SCRIPT
