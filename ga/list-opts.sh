#!/bin/sh

grep -r ga_conf * | gawk '{ if(match($0, "ga_conf[^\\(]+\\(\"([^\"]+)\"", a)) { print substr($0, a[1, "start"], a[1, "length"]); } }' | sort | uniq

