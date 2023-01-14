#!/bin/bash

make clean
make server

rm -rf ourchain_ips.txt
touch ourchain_ips.txt 
./server
