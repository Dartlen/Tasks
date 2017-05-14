#!/bin/sh


for time in `cut -f 2 -d "="  config.ini `
do
   vtime=$time
done

echo "minsk"
while [ : ]
do
	var=$(wget https://pogoda.tut.by/city/minsk -q -O - | grep -Po -m1 '[+-][1-9][0-9]')
	echo "$var"
	sleep $vtime
done
