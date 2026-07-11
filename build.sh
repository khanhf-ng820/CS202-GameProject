# Running CMake build commands in the terminal
cmake -S . -B build -DCMAKE_POLICY_VERSION_MINIMUM=3.5
cmake --build build --config Release --parallel 4
