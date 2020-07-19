#!/bin/bash

FILENAME=''
#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [SWITCHES] [vessel_name]         \n" $0 
	printf "  --help, -h                        \n" 
	exit 0;
    elif [ "${ARGI:0:8}" = "--vname=" ] ; then
	FILENAME="${ARGI#--vname=*}"
    else 
	printf "Bad Argument: %s \n" $ARGI
	exit 0
    fi
done

#-------------------------------------------------------
#  Part 2: Analyze collisions 
#-------------------------------------------------------
alogcd LOG*${FILENAME}*/*.alog > alogcd.txt
cat alogcd.txt
ENCOUNTER=`cat alogcd.txt | grep Encounters: |cut -d' ' -f3`
MISSES=`cat alogcd.txt | grep Misses: |cut -d' ' -f3`
COLLISIONS=`cat alogcd.txt | grep Collisions: |cut -d' ' -f3`
WORST=`cat alogcd.txt | grep Worst: |cut -d' ' -f3`
a=$(($COLLISIONS * 10))
b=$(($MISSES * 5))
c=$(($ENCOUNTER * 2))
SCORE=$(($c - $a ))
SCORE=$(($SCORE - $b))
echo The score for this run is $SCORE