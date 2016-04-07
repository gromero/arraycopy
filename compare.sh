#!/bin/bash

make clean
make
sudo perf record ./arraycopy
cp arraycopy arraycopy_original
mv arraycopy_vsx arraycopy
sudo perf record ./arraycopy
# Present the comparison.
sudo perf diff
# Restore original binary names.
mv arraycopy arraycopy_vsx
mv arraycopy_original arraycopy
rm -fr perf.data*
