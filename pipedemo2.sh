#!/bin/bash
if [[ $# < 1 ]]
then
	printf "Usage: %b repeats\n" $0
	exit
fi
./pipedemo2 $1
printf "Number of X lines : "
	grep X pd2_output | wc -l
printf "Number of y lines : "
	grep y pd2_output | wc -l
printf "X lines in first %b : " $1
	head -$1 pd2_output | grep X | wc -l
printf "y lines in first %b : " $1
	head -$1 pd2_output | grep y | wc -l
printf "X lines in last %b : " $1
	tail -$1 pd2_output | grep X | wc -l
printf "y lines in last %b : " $1
	tail -$1 pd2_output | grep y | wc -l
printf "Xy lines : "
	grep Xy pd2_output | wc -l
printf "yX lines : "
	grep yX pd2_output | wc -l
