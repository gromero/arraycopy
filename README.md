
### Usage

The comparison script will compile non-optimized `-O0` arraycopy() w/ and w/o VSX load/store instruction. After it, `perf record` will be called against both versions and `perf diff` will show finally the differences in execution time:

```
$ ./compare.sh
```

###  Options

`-DVSX`   : compile with VSX instruction
`-DMEMCPY`: compile with memcpy() only, no arraycopy() incorporeted.
`-DCHECK` : enable check if destination data is equal to source data. This is not suitable to use when profiling
