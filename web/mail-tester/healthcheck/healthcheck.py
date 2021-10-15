#!/usr/bin/env python3

from pwn import *
import requests
import re

URL = "http://localhost:3000"

def exploit():
    p = listen(2525)

    s = requests.Session()
    resp = s.get(URL).text
    token = re.search('"authenticity_token" value="(.*)"', resp).group(1)

    resp = s.post(URL + "/login", data={
        "authenticity_token": token,
        "username": "health",
        "password": "check"
    }).text

    token = re.search('"authenticity_token" value="(.*)"', resp).group(1)

    resp = s.post(URL + "/email", data={
        "authenticity_token": token,
        "to": "testing@mail-127-0-0-1.nip.io",
        "message": "hello"
    }).text

    p.wait_for_connection()
    p.sendline("220 hax")
    p.sendlineafter("EHLO", "250 Hi")
    p.sendlineafter("MAIL FROM", "250 Ok")
    p.recvuntil('RCPT TO:<testing@mail-127-0-0-1.nip.io>')
    p.sendline("250 Ok")
    p.sendlineafter("DATA", "354 End data with <CR><LF>.<CR><LF>")
    p.sendlineafter("hello", "250 Ok")
    p.sendlineafter("QUIT", "221 Bye")
    p.close()
    exit(0)


if __name__ == "__main__":
    exploit()
