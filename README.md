### Build and run

Requires CMake 3.25+, Ninja, and vcpkg with `VCPKG_ROOT` set (typically
`VCPKG_ROOT="$HOME/vcpkg"`).

Build presets available: `debug`, `asan` and `release`.

```bash
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
./build/debug/app/hostmon  # press 'q' to quit
```
