#!/bin/bash
for j in `seq 15 21`; do for i in `seq 500`; do numactl --cpunodebind=1 --membind=1 sudo perf stat ./arraycopy_vsx 0 0 ${j} |& fgrep elapsed | tee -a vsx_power_of_${j}.log     ; done ; done
