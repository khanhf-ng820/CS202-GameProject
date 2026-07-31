# Old: Run `make -j4` without running `cmake -B build`/`cmake --build build` again
# cd build
# make -j4
cmake --build build --config Release --parallel 4
