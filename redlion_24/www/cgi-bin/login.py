#!/usr/bin/env python3

import os
import sys
import cgi
import http.cookies
import shelve
import time

SESSION_DB = "/tmp/session_data"
SESSION_TIMEOUT = 600  # 10 minutes

form = cgi.FieldStorage()  # <== You were missing this line
cookie = http.cookies.SimpleCookie(os.environ.get("HTTP_COOKIE", ""))
session_cookie = cookie.get("session_id")

# ======= LOGOUT HANDLER =======
if os.environ.get("REQUEST_METHOD", "") == "POST" and form.getvalue("logout") == "1":
    if session_cookie:
        session_id = session_cookie.value
        with shelve.open(SESSION_DB, writeback=True) as sessions:
            if session_id in sessions:
                del sessions[session_id]
    print("Status: 303 See Other")
    print("Set-Cookie: session_id=; Max-Age=0; HttpOnly")
    print("Location: /login.py")
    print()
    sys.exit(0)


if not session_cookie:
    print("Status: 303 See Other")
    print("Location: /login.py")
    print()
    sys.exit(0)

session_id = session_cookie.value
current_time = time.time()

with shelve.open(SESSION_DB, writeback=True) as sessions:
    session = sessions.get(session_id)

    if not session:
        print("Status: 303 See Other")
        print("Location: /login.py")
        print()
        sys.exit(0)

    if current_time - session["last_seen"] > SESSION_TIMEOUT:
        del sessions[session_id]
        print("Status: 303 See Other")
        print("Set-Cookie: session_id=; Max-Age=0; HttpOnly")
        print("Location: /login.py")
        print()
        sys.exit(0)

    session["last_seen"] = current_time
    session["visits"] += 1
    sessions[session_id] = session

print("Content-Type: text/html")
print(f"Set-Cookie: session_id={session_id}; Max-Age={SESSION_TIMEOUT}; HttpOnly")
print()
print(f"""
<html>
<head><title>Dashboard</title></head>
<body>
    <h1>Welcome, {session['username']}!</h1>
    <p>You have visited this dashboard {session['visits']} times.</p>
    <p>Last active: {time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(session['last_seen']))}</p>
   <form method="POST" action="/logout.py">
    <button type="submit">Logout</button>
</form>
</body>
</html>
""")
