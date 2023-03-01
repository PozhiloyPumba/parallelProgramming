## Paralleling computation of exp(1)
### This my realization of paralleling computation of euler number (with MPI library). 
Dependencies:
- openmpi
- gmp library

It has two version:
1. with additional memory
- You can compile this mode and run with
```
$ make additional_memory
$ mpirun -np "number of cores" ./a.out "accuracy"
```
2. without additional memory
- You can compile this mode and run with
```
$ make
$ mpirun -np "number of cores" ./a.out "accuracy"
```

<img src="../pics/forEuler.jpeg"  width="300" height="300">
