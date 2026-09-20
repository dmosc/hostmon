### Steps to build and run the program

```bash
cmake -B build -S . -G Ninja -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
cmake --build build
./build/hostmon
```
