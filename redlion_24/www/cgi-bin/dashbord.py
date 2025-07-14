#!/usr/bin/env python3

import os
import http.cookies
import time

print("Content-Type: text/html")

# ====== Handle Cookies ======
cookie = http.cookies.SimpleCookie(os.environ.get("HTTP_COOKIE", ""))
session_id = cookie.get("session_id")

if not session_id:
    session_id_value = str(int(time.time()))
    print(f"Set-Cookie: session_id={session_id_value}; Max-Age=600; HttpOnly")
    visit_count = 1
else:
    session_id_value = session_id.value
    visit_count = int(cookie.get("visits", "1").value) + 1
    print(f"Set-Cookie: session_id={session_id_value}; Max-Age=600; HttpOnly")
    print(f"Set-Cookie: visits={visit_count}; Max-Age=600; HttpOnly")

print()  # End of headers
print(f"""
<html>
<head><title>CGI Test</title></head>
<body>
<h1>Hello from CGI</h1>
<p>Session ID: {session_id_value}</p>
<p>Visit count: {visit_count}</p>
</body>
</html>
""")
