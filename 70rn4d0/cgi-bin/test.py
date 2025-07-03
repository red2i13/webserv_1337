#!/usr/bin/env python3

import os
import sys

print("Content-Type: text/html\n")
print("<html><body>")
print("<h1>Hello from Python CGI</h1>")

# GET example
query = os.environ.get("QUERY_STRING", "")
print(f"<p>Query string: {query}</p>")

# POST example
if os.environ.get("REQUEST_METHOD") == "POST":
    length = int(os.environ.get("CONTENT_LENGTH", 0))
    body = sys.stdin.read(length)
    print(f"<p>POST body: {body}</p>")

print("</body></html>")
