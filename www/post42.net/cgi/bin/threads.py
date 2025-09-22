#!/usr/bin/env python3
import os

# Get the list of all files in a directory
path = '../../downloads'

if os.path.exists(path):
    files = os.listdir(path)
    if files:
        res = "["
        for file in files:
            res += '"' + file + '",'
        res = res[:-1] + ']'
    else:
        res = ""
else:
    res = ""
print(res)

