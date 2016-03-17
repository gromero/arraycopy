#!/bin/bash

make clean
make
sudo perf record ./arraycopy
mv arraycopy_vsx arraycopy
sudo perf record ./arraycopy
sudo perf diff
