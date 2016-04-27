#!/bin/bash
for i in `seq 3000`; do numactl --cpunodebind=1 --membind=1 sudo perf stat ./arraycopy_vsx |& fgrep elapsed | tee -a vsx.log     ; done
for i in `seq 3000`; do numactl --cpunodebind=1 --membind=1 sudo perf stat ./arraycopy     |& fgrep elapsed | tee -a original.log; done
