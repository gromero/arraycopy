#!/bin/bash
for i in `seq 3000`; do numactl --cpunodebind=1 --membind=1 sudo perf stat ./arraycopy_vsx |& fgrep elapsed >> vsx.log
for i in `seq 3000`; do numactl --cpunodebind=1 --membind=1 sudo perf stat ./arraycopy     |& fgrep elapsed >> original.log
