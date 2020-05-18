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
ENCOUNTER=`cat alogcd.txt | grep Encounters:`
echo $ENCOUNTER
