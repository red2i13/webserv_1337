#!/usr/bin/env python3
import os
print("Content-Type: text/html\r\n")
print("<html><body><h1>Hello, CGI World!</h1>")
print("<p>Query string: {}</p>".format(os.environ.get("QUERY_STRING", "")))
print("</body></html>")
