Header-only adaptive psychophysical procedures.

API:

- Constructor takes struct b/c huge # of potential parameters; things like:
  - psychometric fn
  - set of intensity values
  - number of trials
  - termination criteria
- `double intensity = next()` returns the next potential value
- `bool continue = update(int value, double intensity)` updates with new data point, returns bool to indicate criterion termination

Algorithms to implement:

- staircase
- quest
- quest+

Extra notes:

- Requirements so far: kthohr/stats? eigen/some other linalg lib?
- Require C++17 (for std::optional, or use Corrade's Containers::Optional for C++11)
- n-dimensional spaces, e.g. like quest+ can handle?
- Want to build python and JS bindings optionally
- Probably going to be GPLv3
- Testing & benchmarking-- Corrade? Google things?
- For emscripten, no `std::optional`? But not sure if it's strictly necessary, and see https://github.com/emscripten-core/emscripten/issues/11139

Notes to self:

```
cmake -S . -B build -DBUILD_TESTS=ON

cmake --build build (--config Release) (--parallel n)
```

Run tests with `ctest -V -C <Debug/Release>` from the build directory.
