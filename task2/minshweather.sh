#!/bin/sh

$1

if [ -z "$*" ]; 
	then $1 set "minsk"; echo "$1"
	else echo "$1"
fi

for  time in `cut -f 2 -d "="  config.ini `
do
   vtime=$time
done

while [ : ]
do
	var=$(wget https://pogoda.tut.by/city/$1 -q -O - | grep -Po -m1 '[+-][1-9][0-9]')
	echo "$var"
	sleep $vtime
done
