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
- Require C++11
- n-dimensional spaces, e.g. like quest+ can handle?
- Want to build python and JS bindings optionally
