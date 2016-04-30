#!/bin/bash
for i in `seq 1000`; do numactl --cpunodebind=1 --membind=1 sudo perf stat ./arraycopy_vsx        0 0 ${j} |& fgrep elapsed | tee -a vsx_${j}.log           ; done 
for i in `seq 1000`; do numactl --cpunodebind=1 --membind=1 sudo perf stat ./arraycopy_vsx_non_pf 0 0 ${j} |& fgrep elapsed | tee -a vsx_non_pf_${j}.log        ; done
for i in `seq 1000`; do numactl --cpunodebind=1 --membind=1 sudo perf stat ./arraycopy            0 0 ${j} |& fgrep elapsed | tee -a original_${j}.log          ; done
