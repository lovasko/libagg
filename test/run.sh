#!/bin/bash
# Copyright (c) 2019 Daniel Lovasko
# All Rights Reserved
#
# Distributed under the terms of the 2-clause BSD License. The full
# license is in the file LICENSE, distributed as part of this software.

c99 -DAGG_DBL -o dbl_test test.c ../impl/agg.c -lm && ./dbl_test
c99 -DAGG_FLT -o flt_test test.c ../impl/agg.c -lm && ./flt_test
