# Error Tables
Each table contains 5 columns, with the first column indicating the population
count, where the four other columns indicate the relevant margin of error given
the type and whether the `-ffast-math` compilation flag was used.

The `-ffast-math` compilation flag causes the following changes:
 * allows reordering of instructions
 * does not set `errno` upon errors
 * does not perform `NaN` checks
 * enables reciprocal approximations for division and square root
 * disables signed zero

Thus, it is possible that the optimization and deviations from the standard
listed above cause the precision of the computations to degrade.

## `fst`
| Count   | `double` | `double fast` | `float` | `float fast` |
+---------+----------+------------ --+---------+--------------+
| `10e1`  | `0.0`    | `0.0`         | `0.0f`  | `0.0f`       |
| `10e2`  | `0.0`    | `0.0`         | `0.0f`  | `0.0f`       |
| `10e3`  | `0.0`    | `0.0`         | `0.0f`  | `0.0f`       |
| `10e4`  | `0.0`    | `0.0`         | `0.0f`  | `0.0f`       |
| `10e5`  | `0.0`    | `0.0`         | `0.0f`  | `0.0f`       |
| `10e6`  | `0.0`    | `0.0`         | `0.0f`  | `0.0f`       |

## `lst`
| Count   | `double` | `double fast` | `float` | `float fast` |
+---------+----------+------------ --+---------+--------------+
| `10e1`  | `0.0`    | `0.0`         | `0.0f`  | `0.0f`       |
| `10e2`  | `0.0`    | `0.0`         | `0.0f`  | `0.0f`       |
| `10e3`  | `0.0`    | `0.0`         | `0.0f`  | `0.0f`       |
| `10e4`  | `0.0`    | `0.0`         | `0.0f`  | `0.0f`       |
| `10e5`  | `0.0`    | `0.0`         | `0.0f`  | `0.0f`       |
| `10e6`  | `0.0`    | `0.0`         | `0.0f`  | `0.0f`       |
