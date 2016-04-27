#!/bin/bash
for i in `seq 10`; do numactl --cpunodebind=1 --membind=1 sudo perf stat ./arraycopy_vsx |& fgrep elapsed ; done | gawk '{v += $1; i++} END {print v/i}'
