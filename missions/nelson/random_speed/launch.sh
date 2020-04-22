#!/bin/bash
#-----------------------------------------------------
# script : launch_vehicle.sh
# Author: John Li
# Date: Apr 2020
# Desrciption: Launch randomly chosen pos
#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
RANDSTART="true"
AMT=4
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [SWITCHES] [time_warp]   \n" $0
	printf "  --just_make, -j    \n" 
	printf "  --help, -h         \n" 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="yes"
    else 
	printf "Bad Argument: %s \n" $ARGI
	exit 0
    fi
done

#-------------------------------------------------------------
# Part 3: Generate random starting positions, speeds and vnames

#-------------------------------------------------------------
if [ "${RANDSTART}" = "true" ] ; then
    pickpos --poly="60,10:80,10:80,-10:60,-10"      \
            --poly="90,10:110,10:110,-10:90,-10"  \
            --poly="60,-140:80,-140:80,-160:60,-160"  \
	    --poly="90,-140:110,-140:110,-169:90,-160"\
	    --amt=$AMT  > vpositions.txt  

    pickpos --amt=$AMT --spd=1:5 > vspeeds.txt
    
fi

VEHPOS=(`cat vpositions.txt`)
SPEEDS=(`cat vspeeds.txt`)
#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#-------------------------------------------------------
VNAME1="nelson"         # The testing vehicle Community
VNAME2="alpha"          # The second vehicle Community
VNAME3="bravo"          # The third vehicle Community
VNAME4="charlie"        # The fourth vehicle Community
VNAME5="delta"          # The fifth vehicle Community

START_POS1="-20,-75"      # Start in the middle
START_POS2="70,0"
START_POS3="95,0"
START_POS4="70,-150"
START_POS5="95,-150"

START_HEADING1="90"
START_HEADING2="180"
START_HEADING3="180"
START_HEADING4="0"
START_HEADING5="0"

LOITER_POS1="180,-75"
LOITER_POS2="70,-150"
LOITER_POS3="95,-150"
LOITER_POS4="70,0"
LOITER_POS5="95,0"

TRANSIT_SPEED1="1.5"
TRANSIT_SPEED2=${SPEEDS['1']}
TRANSIT_SPEED3=${SPEEDS['2']}
TRANSIT_SPEED4=${SPEEDS['3']}
TRANSIT_SPEED5=${SPEEDS['4']}

SHORE_LISTEN="9300"

#The testing vehicle's metafiles are called my_vehicle.moos/bhv
nsplug my_vehicle.moos targ_$VNAME1.moos -f WARP=$TIME_WARP \
    VNAME=$VNAME1          SHARE_LISTEN="9301"              \
    VPORT="9001"           SHORE_LISTEN=$SHORE_LISTEN       \
    START_POS=$START_POS1  START_HEADING=$START_HEADING1

nsplug meta_vehicle.moos targ_$VNAME2.moos -f WARP=$TIME_WARP \
    VNAME=$VNAME2          SHARE_LISTEN="9302"              \
    VPORT="9002"           SHORE_LISTEN=$SHORE_LISTEN       \
    START_POS=$START_POS2  START_HEADING=$START_HEADING2

nsplug meta_vehicle.moos targ_$VNAME3.moos -f WARP=$TIME_WARP \
    VNAME=$VNAME3          SHARE_LISTEN="9303"              \
    VPORT="9003"           SHORE_LISTEN=$SHORE_LISTEN       \
    START_POS=$START_POS3  START_HEADING=$START_HEADING3
    
nsplug meta_vehicle.moos targ_$VNAME4.moos -f WARP=$TIME_WARP \
    VNAME=$VNAME4          SHARE_LISTEN="9304"              \
    VPORT="9004"           SHORE_LISTEN=$SHORE_LISTEN       \
    START_POS=$START_POS4  START_HEADING=$START_HEADING4

nsplug meta_vehicle.moos targ_$VNAME5.moos -f WARP=$TIME_WARP \
    VNAME=$VNAME5          SHARE_LISTEN="9305"              \
    VPORT="9005"           SHORE_LISTEN=$SHORE_LISTEN       \
    START_POS=$START_POS5  START_HEADING=$START_HEADING5
    
nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
    SNAME="shoreside"  SHARE_LISTEN=$SHORE_LISTEN                 \
    SPORT="9000"

nsplug my_vehicle.bhv targ_$VNAME1.bhv -f VNAME=$VNAME1     \
    START_POS=$START_POS1 LOITER_POS=$LOITER_POS1           \
    TRANSIT_SPEED=$TRANSIT_SPEED1

nsplug meta_vehicle.bhv targ_$VNAME2.bhv -f VNAME=$VNAME2   \
    START_POS=$START_POS2 LOITER_POS=$LOITER_POS2           \
    TRANSIT_SPEED=$TRANSIT_SPEED2

nsplug meta_vehicle.bhv targ_$VNAME3.bhv -f VNAME=$VNAME3   \
    START_POS=$START_POS3 LOITER_POS=$LOITER_POS3           \
    TRANSIT_SPEED=$TRANSIT_SPEED3 

nsplug meta_vehicle.bhv targ_$VNAME4.bhv -f VNAME=$VNAME4   \
    START_POS=$START_POS4 LOITER_POS=$LOITER_POS4           \
    TRANSIT_SPEED=$TRANSIT_SPEED4

nsplug meta_vehicle.bhv targ_$VNAME5.bhv -f VNAME=$VNAME5   \
    START_POS=$START_POS5 LOITER_POS=$LOITER_POS5           \
    TRANSIT_SPEED=$TRANSIT_SPEED5

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
printf "Launching $SNAME MOOS Community (WARP=%s) \n"  $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
printf "Launching $VNAME1 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_$VNAME1.moos >& /dev/null &
printf "Launching $VNAME2 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_$VNAME2.moos >& /dev/null &
printf "Launching $VNAME1 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_$VNAME3.moos >& /dev/null &
printf "Launching $VNAME2 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_$VNAME4.moos >& /dev/null &
printf "Launching $VNAME1 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_$VNAME5.moos >& /dev/null &

printf "Done \n"

uMAC targ_shoreside.moos

printf "Killing all processes ... \n"
kill %1 %2 %3 %4 %5 %6
printf "Done killing processes.   \n"
