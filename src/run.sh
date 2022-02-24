#!/bin/bash
make clean
make
echo 'start running'
./proxy
while true
do
	sleep 1
done
