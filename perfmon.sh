#!/usr/bin/env bash
perf stat -e cpu-cycles -e r5300c7 -e r5301c7 -e r5302c7 -e r5303c7 -e r5304c7 -e r5305c7 -e r5306c7 -e r5307c7  ./prog
