#!/usr/bin/env python3
import os

# Get the list of all files in a directory
path = './www/post42.net/threads'
files = os.listdir(path)
res = "["
for file in files:
    res += '"'
    res += file
    res += '",'
res = res[:-1] + ']'
print(res)