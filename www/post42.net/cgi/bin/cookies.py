#!/usr/bin/env python3
import os
import http.cookies
import cgi
import uuid
import sys
import json

try:
    data = json.load(sys.stdin)
    action = data.get("action")
except Exception:
    action = None

if action == "start":
    session_id = str(uuid.uuid4())
    cookie = http.cookies.SimpleCookie()
    cookie["sessionid"] = session_id
    cookie["sessionid"]["path"] = "/"
    cookie["sessionid"]["max-age"] = 3600
    print(cookie.output())

elif action == "end":
    cookie = http.cookies.SimpleCookie()
    cookie["sessionid"] = ""
    cookie["sessionid"]["path"] = "/"
    cookie["sessionid"]["expires"] = "Thu, 01 Jan 1970 00:00:00 GMT"
    print(cookie.output())

else:
    print("No action.")