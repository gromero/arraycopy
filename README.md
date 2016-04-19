```
for i in `seq 10`; do time numactl --cpunodebind=16 --membind=16 ./arraycopy_vsx ; done |& fgrep real
```
