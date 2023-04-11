#!/bin/bash

for (( c=2; c<=500000; c *= 2 ))
do
	gcc sequential.c -DQUIET -DK=$c -O3 -lm -o sequence.out
	time ./sequence.out
done

for (( c=2; c<=500000; c *= 2 ))
do
	echo ---------
	echo one
	mpicc parallel.c -DQUIET -DK=$c -O3 -lm -o par.out
	time mpirun -n 1 ./par.out
	echo four
	mpicc parallel.c -DQUIET -DK=$c -O3 -lm -o par.out
	time mpirun -n 4 ./par.out
done


