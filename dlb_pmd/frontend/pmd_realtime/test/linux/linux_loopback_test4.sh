#!/bin/bash
#
# simple loop test: play a file into a loopback device, and read it back
#
# To make this work, add 5 ALSA loopback virtual sound cards:
#
#     1. create new file /etc/modprobe.d/sound.conf containing the single line
#
#            options snd-aloop enable=1,1,1,1,1 index=10,11,12,13,14
#
#        (no # at beginning of this line, 1st character should be 'o' for options)
#
#     2. insert the snd-aloop module:
#
#            $ sudo modprobe snd_aloop
#
# Assuming these steps work (otherwise you may need to reboot), you can assume 5
# loopback virtual devices with 'hw' numbers 10,11,12,13,14
#
# (Note that we don't actually need 5 devices for this test -- 2 will do -- but
# we may want more loopback devices for something else).
#
# Note also that this file assumes a 6-channel input .wav file called '6.wav'

PORT=54321
SVC=metadata


PLAYOUT1=`./pmd_realtime_release | grep hw:10,0 | awk '{print $2}' | cut -c-2`
CAPIN1=`./pmd_realtime_release | grep hw:10,1 | awk '{print $2}' | cut -c-2`
PLAYOUT2=`./pmd_realtime_release | grep hw:11,0 | awk '{print $2}' | cut -c-2`
CAPIN2=`./pmd_realtime_release | grep hw:11,1 | awk '{print $2}' | cut -c-2`


./pmd_realtime_debug play 6.wav -od $((10#$PLAYOUT1)) -mdi 1.xml -chan 5 &
PLAY1_PID=$!

./pmd_realtime_release play 6.wav -od $((10#$PLAYOUT2)) -listen $PORT $SVC -chan 5 &
PLAY2_PID=$!
sleep 1

./pmd_realtime_debug capture /dev/null -id $((10#$CAPIN1)) -mdo http://localhost:$PORT/$SVC -cc 6 -chan 5 &
CAP1_PID=$!

./pmd_realtime_release capture cap.wav -id $((10#$CAPIN2)) -mdo cap.xml -cc 6 -chan 5 &
CAP2_PID=$!

echo waiting for file-input player to stop
wait $PLAY1_PID
echo file-input player stopped
kill -INT $CAP1_PID 
echo waiting for 1st capture 
wait $CAP1_PID
echo 1st capture stopped

echo waiting for listener player
wait $PLAY2_PID
echo listening player stopped
kill -INT $CAP2_PID 
echo waiting for 2nd capture
wait $CAP2_PID
echo 2nd capture stopped
