#!/bin/bash

make

for ((i = 128; i < 100000; i *= 2))
do
	echo -------- $i ----------
	echo sequence
	time ./seq.out $i
	echo parallel
	for ((j = 1; j < 5; ++j))
	do
		echo par with $j threads
		time mpirun -n $j ./par.out $i 
	done
done
