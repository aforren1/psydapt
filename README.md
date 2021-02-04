Header-only adaptive psychophysical procedures.

Notes to self for building:

```
cmake -S . -B build -DPSYDAPT_BUILD_TESTS=ON

cmake --build build (--config Release) (--parallel n)
```

Run tests with `ctest -V -C <Debug/Release>` from the build directory.
