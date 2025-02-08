#!/usr/bin/env python3

import os
import http.cookies
import cgi

# Read cookies from the environment
cookie_string = os.environ.get('HTTP_COOKIE', '')
cookies = http.cookies.SimpleCookie(cookie_string)

# Read session ID from cookies
session_id = cookies.get('SESSIONID')

# Simulate session storage (in a real application, use a database or file storage)
session_data = {
    "session1": {"username": "JohnDoe"},
    "session2": {"username": "JaneDoe"}
}

# Get session data
username = "Guest"
if session_id and session_id.value in session_data:
    username = session_data[session_id.value]["username"]

# Generate a new session ID if not present
if not session_id:
    import uuid
    session_id = str(uuid.uuid4())
    cookies['SESSIONID'] = session_id
    cookies['SESSIONID']['path'] = '/'
    cookies['SESSIONID']['max-age'] = 3600  # 1 hour

# Output the HTTP headers
print("Content-Type: text/html")
print(cookies.output())
print()

# Output the HTML content
print(f"<html><body>")
print(f"<h1>Hello, {username}!</h1>")
print(f"<p>Your session ID is: {session_id}</p>")
print(f"</body></html>")