#!/bin/bash

TEMPF="$(mktemp)"

/usr/bin/time -v -o "$TEMPF" -- g++ "$@"

echo $(cat "$TEMPF" | grep "Maximum resident set size") : "$@"
rm -f "$TEMPF"
