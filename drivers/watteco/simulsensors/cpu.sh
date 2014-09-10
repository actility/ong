#!/bin/bash
read cpu a b c previdle rest < /proc/stat
prevtotal=$((a+b+c+previdle))
sleep 1.0
read cpu a b c idle rest < /proc/stat
total=$((a+b+c+idle))
CPU=$((100*( (total-prevtotal) - (idle-previdle) ) / (total-prevtotal) ))
echo $CPU
