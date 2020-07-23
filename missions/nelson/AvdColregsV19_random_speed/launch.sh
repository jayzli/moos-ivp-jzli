#!/bin/bash
#-----------------------------------------------------
# script : launch_vehicle.sh
# Author: John Li
# Date: April 2020
# Desrciption: Launch a specified number of vehicles from four random polygons
#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
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
    elif [ "${ARGI:0:6}" = "--amt=" ] ; then                      
        AMT="${ARGI#--amt=*}"                                     
        if [ ! $AMT -ge 4 ] ; then                                
            echo "launch.sh: Vehicle amount must be >= 4."        
            exit 1                                                
        fi
    else 
	printf "Bad Argument: %s \n" $ARGI
	exit 0
    fi
done

#-------------------------------------------------------------
# Part 2: Generate Shore files and launch shoreside
#-------------------------------------------------------------
SHORE_LISTEN="9300"

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
    SNAME="shoreside"  SHARE_LISTEN=$SHORE_LISTEN                 \
    SPORT="9000"

if [ ${JUST_MAKE} = "no" ] ; then
    printf "Launching shoreside MOOS Community (WARP=%s) \n"  $TIME_WARP
    pAntler targ_shoreside.moos >& /dev/null &
fi

#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files for testing vehicle
# The testing vehicle's metafiles are called my_vehicle.moos/bhv
# Launch ownship nelson
#-------------------------------------------------------
VNAME1="nelson"         # The testing vehicle Community
START_POS1="x=-200,y=-75"      # Start in the middle
START_HEADING1=$(expr 90 + $((RANDOM % 20 - 10)))
START1="${START_POS1}, heading=${START_HEADING1}"

nsplug my_vehicle.moos targ_$VNAME1.moos -f WARP=$TIME_WARP \
    VNAME=$VNAME1          SHARE_LISTEN="9301"              \
    VPORT="9001"           SHORE_LISTEN=$SHORE_LISTEN       \
    GROUP='own'            START_POS=$START1 #for uSimMarine

TRANSIT_SPEED1=$((RANDOM % 5 + 3))
LOITER_POS1="370,-75"
TRANSIT_SPEED2=(`pickpos --amt=1 --spd=1:5`)
nsplug my_vehicle.bhv targ_$VNAME1.bhv -f VNAME=$VNAME1     \
    START_POS=$START1 LOITER_POS=$LOITER_POS1           \
    TRANSIT_SPEED=$TRANSIT_SPEED2

if [ ${JUST_MAKE} = "no" ] ; then
    printf "Launching $VNAME1 MOOS Community (WARP=%s) \n" $TIME_WARP
    pAntler targ_$VNAME1.moos >& /dev/null &
fi

#-------------------------------------------------------
#  Part 4: Create the .moos and .bhv files for other vehicles
#  Launch rest of the vehicles
#-------------------------------------------------------
printf "Generating $AMT contacts with random heading and speeds\n"
pickpos --amt=$AMT --vnames  > vnames.txt  
pickpos --amt=$AMT --spd=1:5 > vspeeds.txt
SPEEDS=(`cat vspeeds.txt`)                                              
VNAMES=(`cat vnames.txt`)                                               
STARTPOLY=(`cat startpolygons.txt`)  #start polygons are set by user in txt file
STARTNUM=`wc -l <startpolygons.txt` #number of starting polygons
LOITERPOLY=(`cat loiterpolygons.txt`)  #loiter polygons are set by user in txt file
LOITERNUM=`wc -l <loiterpolygons.txt` #number of loiter polygons
GROUP='contacts'
sleep 1

for INDEX in `seq 1 $AMT`;
do
    sleep 0.5
    ARRAY_INDEX=`expr $INDEX - 1` # array start at 0
    PORT_INDEX=`expr $INDEX + 1` # for example, the first contact need to take port 2
    START_INDEX=`expr $ARRAY_INDEX % $STARTNUM`
    LOITER_INDEX=`expr $ARRAY_INDEX % $LOITERNUM`

    START=`pickpos --poly="${STARTPOLY[START_INDEX]}" --amt=1`
    LOITER_POS=`pickpos --poly="${LOITERPOLY[LOITER_INDEX]}" --amt=1`
    
    VNAME=${VNAMES[$ARRAY_INDEX]}                                       
    SPEED=${SPEEDS[$ARRAY_INDEX]}
    VPORT=`expr $PORT_INDEX + 9000`
    LPORT=`expr $PORT_INDEX + 9300`
    
    nsplug meta_vehicle.moos targ_$VNAME.moos -f WARP=$TIME_WARP \
	   VNAME=$VNAME          SHARE_LISTEN=$LPORT              \
	   VPORT=$VPORT           SHORE_LISTEN=$SHORE_LISTEN       \
	   START_POS=$START       GROUP=$GROUP
    nsplug meta_vehicle.bhv targ_$VNAME.bhv -f VNAME=$VNAME   \
	   START_POS=$START  LOITER_POS=$LOITER_POS           \
	   TRANSIT_SPEED=$SPEED
    printf "Launching $VNAME MOOS Community (WARP=%s) \n" $TIME_WARP
    pAntler targ_$VNAME.moos >& /dev/null &
done

printf "Done \n"
#-------------------------------------------------------
#  Part 4: start uMAC
#-------------------------------------------------------
uMAC targ_shoreside.moos

printf "Killing all processes ... \n"
ktm
printf "Done killing processes.   \n"
