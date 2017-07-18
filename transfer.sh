#!/bin/bash

if [ $# -ne 1 ] 
then
	echo "usage $0: <name of sd card>"
	exit
fi

make
cp kernel7.img /media/$1
sync
