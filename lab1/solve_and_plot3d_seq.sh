#!/bin/bash

gcc sequential.c -lm
./a.out > out
python3 plot3d.py
