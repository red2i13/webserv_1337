# #!/usr/bin/env python3

# import os
# import http.cookies
# import time

# print("Content-Type: text/html")

# # ====== Handle Cookies ======
# cookie = http.cookies.SimpleCookie(os.environ.get("HTTP_COOKIE", ""))
# session_id = cookie.get("session_id")

# if not session_id:
#     session_id_value = str(int(time.time()))
#     print(f"Set-Cookie: session_id={session_id_value}; Max-Age=600; HttpOnly")
#     visit_count = 1
# else:
#     session_id_value = session_id.value
#     visit_count = int(cookie.get("visits", "1").value) + 1
#     print(f"Set-Cookie: session_id={session_id_value}; Max-Age=600; HttpOnly")
#     print(f"Set-Cookie: visits={visit_count}; Max-Age=600; HttpOnly")

# print()  # End of headers
# print(f"""
# <html>
# <head><title>CGI Test</title></head>
# <body>
# <h1>Hello from CGI</h1>
# <p>Session ID: {session_id_value}</p>
# <p>Visit count: {visit_count}</p>
# </body>
# </html>
# """)


#!/usr/bin/env python3

import os
import http.cookies
import time
import shelve

SESSION_DB = "/tmp/session_data"
SESSION_TIMEOUT = 600  # 10 minutes

# print("Content-Type: text/html")

cookie = http.cookies.SimpleCookie(os.environ.get("HTTP_COOKIE", ""))
session_cookie = cookie.get("session_id")

if not session_cookie:
    # No session cookie, redirect to login
    print("Status: 303 See Other")
    print("Location: /cgi-bin/login.py")
    print()
    exit(0)

session_id = session_cookie.value
current_time = time.time()

with shelve.open(SESSION_DB, writeback=True) as sessions:
    session = sessions.get(session_id)

    if not session:
        # Invalid session, redirect to login
        print("Status: 303 See Other")
        print("Location: /cgi-bin/login.py")
        print()
        exit(0)

    if current_time - session["last_seen"] > SESSION_TIMEOUT:
        # Session expired, delete session, redirect to login
        del sessions[session_id]
        print("Status: 303 See Other")
        print("Set-Cookie: session_id=; Max-Age=0; HttpOnly; Path=/")
        print("Location: /cgi-bin/login.py")
        print()
        exit(0)

    # Update session activity and visits
    session["last_seen"] = current_time
    session["visits"] += 1
    sessions[session_id] = session

# Set cookie with updated expiry
print(f"Set-Cookie: session_id={session_id}; Max-Age={SESSION_TIMEOUT}; HttpOnly; Path=/")
print()

print(f"""
<html>
<head><title>Dashboard</title></head>
<body>
    <h1>Welcome, {session['username']}!</h1>
    <p>You have visited this dashboard {session['visits']} times.</p>
    <p>Last active: {time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(session['last_seen']))}</p>
    <form method="POST" action="/cgi-bin/dashbord.py">
        <input type="hidden" name="logout" value="1">
        <button type="submit">Logout</button>
    </form>
</body>
</html>
""")
