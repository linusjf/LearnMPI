#!/bin/bash
#by Nacer Salah eddine


#Script Used for compiling and running openmpi programs
clear
source_file_name="./reduce.cpp"
output_file_name="./reduce.exe"
no_of_process=20

echo ---------Compiling Program------------

mpic++ $source_file_name  -o $output_file_name -Wall
if [ $? == 0 ];then #No Errors Detected
	echo "COMPILE SUCCESS"
	echo ---------Executing Program------------
    termux-elf-cleaner $output_file_name &> /dev/null 
    mpirun --allow-run-as-root --hostfile ../hostfile -np $no_of_process --oversubscribe $output_file_name
else
	echo "COMPILE ERROR"
fi


#END OF SCRIPT
