# libagg
The `libagg` is a C99 library that implements a set of algorithms that provide statistical
aggregation of a stream of floating-point numerical values. All aggregate functions are provided in
two separate implementations: _static_ and _streaming_.

The two key properties achieved by the streaming implementations relate to the fact that
the full set of values does not need to be known ahead, enabling:
 * large data sets to be processed in constant amount of memory
 * analysis of data sets of unknown length (e.g. user input, non-deterministic behaviour of a
   system)

These abilities come at a cost: the precision of the streaming implementation is reduced compared to
the static implementation. This trade-off is closely monitored by the test suite to document and
empirically prove the magnitude of the error for each aggregate function.

## Aggregate Functions
The module provides the following aggregate functions:
 * first value
 * last value
 * count
 * sum
 * minimum
 * maximum
 * average
 * variance
 * standard deviation
 * skewness
 * kurtosis
 * p-quantile
 * median

## API
### Functions
The streaming part of the library consists of the following three functions:
 * `agg_new` to initialize or reset the state
 * `agg_put` to update the statistical aggregate estimate
 * `agg_get` to obtain the statistical aggregate estimate

The static part of the library consists only of one function:
 * `agg_run` to calculate the statistical aggregate

### Types
The streaming part of the library consists only of one type:
  * `struct agg` which keeps track of state and should be treated as an opaque structure

The static part of the library does not use any custom types.

### Constants
The following constants are used to identify the aggregate functions by both parts:
 * `AGG_FNC_FST` for first value
 * `AGG_FNC_LST` for last value
 * `AGG_FNC_CNT` for count
 * `AGG_FNC_SUM` for sum
 * `AGG_FNC_MIN` for minimum
 * `AGG_FNC_MAX` for maximum
 * `AGG_FNC_AVG` for average
 * `AGG_FNC_VAR` for variance
 * `AGG_FNC_DEV` for standard deviation
 * `AGG_FNC_SKW` for skewness
 * `AGG_FNC_KRT` for kurtosis
 * `AGG_FNC_QTL` for p-quantile
 * `AGG_FNC_MED` for median

## Examples
The following snippet computes the 99th percentile of values in an stream whilst retrieving numbers
from the hypothetical stream by calling the `get_number` function.
```c
struct agg agg;
double     num;
double     p99;
bool       ret;
uint8_t    idx;

agg_new(&agg, AGG_FNC_QTL, 0.99);

for (idx = 0; idx < 32; idx++) {
  num = get_number();
  agg_put(&agg, num);
}

ret = agg_get(&agg, &p99);
```

A similar computation can be performed using the static algorithm that expects the full stream
knowledge ahead:
```c
double  num[32];
double  var;
bool    ret;
uint8_t idx;

for (idx = 0; idx < 32; idx++) {
  num[idx] = get_number();
}

ret = agg_run(&p99, num, 32, AGG_FNC_QTL, 0.99);
```

## Floating-Point Types
The library uses the `double` type by default, as it is the de-facto standard floating-point type
in the C99 language. This is evidenced by the fact that all mathematical functions for `float` and
`long double` are differentiated by a suffix, e.g. `sin`, `sinf`, and `sinl`. Moreover, the
numerical literals in the language are `double` by default too.

In order to use other floating point types instead, the `AGG_BIT` macro with the appropriate bit width
has to be defined before the `agg.h` header file is included (and thus attended to by the pre-processor).
This creates a trade-off, where the precision of all functions The switch is not dynamic and has to
be done in during the compilation of the source code. The following table lists currently supported
floating point types and their respective values recognised by `AGG_BIT`:

| Type         | `AGG_BIT` |
|--------------|-----------|
| `float`      | 32        |
| `double`     | 64        |
| `__float128` | 128       |

## Testing
The library has a particular trade-off at its heart: it sacrifices the precision of the
computations in order to provide the streaming capabilities of the aggregate functions. With the
explicit goal of keeping this trade-off in check, a suite of tests was introduced that compare the
streaming variants of functions to their non-streaming static versions and compute the difference
between the two. Furthermore, each aggregate function specifies its accepted magnitude of error
for a given number of incoming values. The magnitudes are always the upper bound, rounded to the
closes power of ten - both positive and negative.

The precise values can be found in the [ERROR.md](ERROR.md) file.

## Memory Usage
The library does not dynamically allocate any memory and thus all aggregations are performed in a
constant amount of statically allocated memory on the stack. Based on the chosen floating-point
type - `double` or `float` -  the core type `struct agg` takes up 92 and 136 bytes, respectively.

## Performance
Vast majority of the code is branchless and hand-optimized for performance. The test suite measures
the average execution time per a single value, which tends to be in the order of nanoseconds.

The measurements show stable performance with almost no variance, which makes the library suitable
for use in low-latency scenarios.

## Note on Optimizations
All major C99 compilers offer multiple optimization levels, some of which might sacrifice the
correctness of the computation in order to achieve better performance. The `-ffast-math` option,
which is part of the `-Ofast` optimization level, causes a number of changes to the behaviour of the
floating-point computations. This in turn causes slight divergence in the numerical precision of
the algorithms in questions. The error testing takes this into account and monitors the skew
appropriately.

The [ERROR.md](ERROR.md) file contains the columns `double fast` and `float fast` that represent
the `-Ofast` compilation option.

## Note on Randomness
All tests use a very weak source of randomness: a simple linear congruential generator that is not
to be used in a serious production setting where either cryptographic safety or perfect
distribution are of the essence.

The reason for depending on the weak generator is that of dependencies of the module: one of the
design goals of the module is to be extremely light. The standard `rand` function was not used in
order to silence static analysis warnings.

## Standards
Both the library and test suite are written in standards-compliant C99. The provided source code
ought to compile on all standard compilers without any warnings. Reports of any compiler or static
analysis warnings is encourage and will be addressed.

## Future Work
The following areas of focus are not addressed by the library at this time:
  * support for the `long double` time
  * inter-quartile range aggregate function
  * headless mode where the function type is not stored as part of `struct agg`
  * ability to select an integer type size for the count variables

## License
The module is licensed under the 2-clause BSD license (see LICENSE file for more information). In
case you need a different license, feel free to contact the author.

## Author
Daniel Lovasko <daniel.lovasko@gmail.com>
