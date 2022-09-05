#!/bin/bash
#
# simple loop test: play a file into a loopback device, and read it back, saving metadata
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
# (Note that we don't actually need 5 devices for this test -- 1 will do -- but
# we may want more loopback devices for something else).
#
# Note also that this file assumes a 6-channel input .wav file called '6.wav'

PLAYOUT=`./pmd_realtime_release | grep hw:10,0 | awk '{print $2}' | cut -c-2`
CAPIN=`./pmd_realtime_release | grep hw:10,1 | awk '{print $2}' | cut -c-2`

PORT=54321
SVC=metadata

./pmd_realtime_release play 6.wav -od $((10#$PLAYOUT)) -listen $PORT $SVC -chan 5 &
sleep 1
PLAY_PID=$!

./pmd_realtime_release capture cap.wav -id $((10#$CAPIN)) -mdo cap.xml -cc 6 -chan 5 &
CAP_PID=$!
sleep 1

if curl -X POST --data-binary @1.xml --header "Content-Type:application/xml; charset=utf-8" http://localhost:$PORT/$SVC
then
    echo POST succeeded
else
    echo POST failed
fi

wait $PLAY_PID
kill -INT $CAP_PID 
wait $CAP_PID
