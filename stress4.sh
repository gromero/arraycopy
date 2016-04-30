#!/bin/bash
for i in `seq 1000`; do numactl --cpunodebind=1 --membind=1 sudo perf stat ./arraycopy_vsx |& fgrep elapsed | tee -a vsx_pf_deepest.log      ; done 
for i in `seq 1000`; do numactl --cpunodebind=1 --membind=1 sudo perf stat ./arraycopy     |& fgrep elapsed | tee -a original_pf_deepest.log ; done
