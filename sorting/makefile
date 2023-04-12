all: seq par

seq:
	gcc oddEvenBase.c sequenceOddEven.c -o seq.out

par:
	mpicc oddEvenBase.c parallelOddEven.c -o par.out

clean:
	rm -rf *.out
