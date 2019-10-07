# libagg
The `libagg` library implements a set of algorithms that provide statistical
aggregation of a stream of values. All such algorithms are _on-line_, meaning
that the stream does not need to be known ahead, potentially allowing for
massive data sets to be processed in constant amount of memory.

## Aggregate Functions
The module provides the following aggregate functions: first, last, minimum,
maximum, average, and variance.

## API
TODO

## Examples
TODO

## Standards
TODO

## Testing
TODO

### Error
This module has a particular trade-off at its heart: it sacrifices the
precision of the computations in order to provide the streaming capabilities of
the aggregate values. With the explicit goal of keeping this trade-off in
check, a suite of tests was introduced that compare the streaming variants of
functions to their non-streaming ideal versions and compute the statistical
error between the two. Furthermore, each aggregate function specifies its
accepted margin of error for a given number of incoming values.

### Performance
TODO

### Memory Usage
TODO

## Note on Precision
Whilst the statistical error of a particular computation might be
insignificant, the final quality of the computation can be affected by the
chosen type. The following example juxtaposes the results utilizing four
different types:

```
float    = 4.7192497
double   = 4.719250462962962
```

## Note on Randomness
All tests use a very weak source of randomness: a simple linear congruential
generator that is not to be used in a serious production setting where either
crypto-graphical safety or perfect .

The reason for such trade-off is that of dependencies of the module:
one of the design goals of the module is to be extremely light.

## License
The module is licensed under the 2-clause BSD license (see LICENSE file
for more information). In case you need a different license, feel free to
contact the author.

## Author
Daniel Lovasko <daniel.lovasko@gmail.com>
