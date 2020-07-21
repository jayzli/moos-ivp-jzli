#!/bin/bash -e
source ~/.bashrc
cd /Users/johnli/moos-ivp-jzli/missions/nelson/AvdColregsV19_random_speed/
#----------------------------------------------------------
#  Script: launch.sh
#  Author: Michael Benjamin
#  LastEd: May 17th 2019
#----------------------------------------------------------
#  Part 1: Set Exit actions and declare global var defaults
#----------------------------------------------------------
trap "kill -- -$$" EXIT SIGTERM SIGHUP SIGINT SIGKILL
TIME_WARP=15
FLOW_DOWN_ARGS=""
AMT=200
MAX_TIME=9000
#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "xlaunch.sh [SWITCHES] [time_warp]     "
	echo "  --help, -h                          " 
	echo "  --just_make, -j                     " 
	echo "  --no_gui, -n     (no pMarineViewer) " 
	echo "  --amt=N          (total encounters) " 
	echo "  --mt=secs        (max time in secs) " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
	TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--no_gui" -o "${ARGI}" = "-n" ] ; then
	FLOW_DOWN_ARGS+="${ARGI} "
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ] ; then
	FLOW_DOWN_ARGS+="${ARGI} "
    elif [ "${ARGI:0:6}" = "--amt=" ] ; then
        AMT="${ARGI#--amt=*}"
    elif [ "${ARGI:0:5}" = "--mt=" ] ; then
        MAX_TIME="${ARGI#--mt=*}"
    else 
	echo "xlaunch.sh: Bad Arg:" $ARGI
	exit 0
    fi
done

#-------------------------------------------------------
#  Part 3: Launch mission, note PID for later killing
#-------------------------------------------------------
./launch2.sh $FLOW_DOWN_ARGS $TIME_WARP >& /dev/null &
LAUNCH_PID=$!

#-------------------------------------------------------
#  Part 4: Start the mission with the right pokes
#-------------------------------------------------------
sleep 5
echo "Poking...." 
uPokeDB targ_shoreside.moos DEPLOY_ALL=true MOOS_MANUAL_OVERRIDE_ALL=false

#-------------------------------------------------------
#  Part 5: Monitor the mission
#-------------------------------------------------------
while [ 1 ] ; do 
    sleep 10
    if uQueryDB targ_shoreside.moos --condition="ENCOUNTER_TOTAL > $AMT" ; then 
	break;
    else
	sleep 10
	if uQueryDB targ_shoreside.moos --condition="DB_UPTIME > $MAX_TIME" ; then 
	    break;
	fi
    fi
done

#-------------------------------------------------------
#  Part 6: Bring down all the mission modules
#-------------------------------------------------------
echo "Quitting...."
kill -9 $LAUNCH_PID
ktm
#-------------------------------------------------------
#  Part 7: Analyze and store results
#-------------------------------------------------------
./analysis.sh > result.txt
#-------------------------------------------------------
#  Part 8: Save the logs in a dedicated folder
#-------------------------------------------------------
RESULTS_DIR="results_"`date "+%Y_%m_%d_____%H_%M"`
mkdir $RESULTS_DIR
mv targ* *LOG* $RESULTS_DIR
mv result.txt $RESULTS_DIR
echo "Completely done with xlaunch."

# HELM_BHV_PARAM = type=AvdColregsV19, name=avd_col_foo, param=pwt_outer_dist, value=35
