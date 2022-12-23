#!/usr/bin/env bash
rm -rf build && mkdir build && cd build && cmake .. && cmake --build . --target test_dilithium2_ref_nolib && ./ref/test_dilithium2_ref_nolib && cd ..
