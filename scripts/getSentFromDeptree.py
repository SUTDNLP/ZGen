#!/usr/bin/env python
import sys

fp = open(sys.argv[1], "r")
for instance in fp.read().strip().split("\n\n"):
    words = []
    for line in instance.split("\n"):
        form = line.strip().split()[0]
        words.extend(form.strip("__").split("_"))
    print " ".join(words)
