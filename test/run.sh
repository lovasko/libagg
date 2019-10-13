#!/bin/bash

c99 -DAGG_DBL -o dbl_test test.c ../impl/agg.c -lm && ./dbl_test
c99 -DAGG_FLT -o flt_test test.c ../impl/agg.c -lm && ./flt_test
