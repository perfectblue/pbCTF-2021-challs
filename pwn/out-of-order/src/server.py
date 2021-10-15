#!/usr/bin/env python3
import os
import sys
import signal
import random
import hashlib
import resource

signal.alarm(100)
resource.setrlimit(resource.RLIMIT_AS, (100000000, 100000000))

random.seed()
nonce = str(random.randint(111111111111111,999999999999999))

print(f"PoW: Give me x where sha256('{nonce}' + x) starts with 0000000")
x = input().rstrip()

m = hashlib.sha256()
m.update((nonce + x).encode('ascii'))
h = m.digest().hex()
if not h.startswith('0000000'):
    print(f'Bad PoW. Got hash: {h}')
    exit(0)

print('PoW accepted, thank you')
sys.stdout.flush()

os.chdir('/home/chal')
os.chroot('.')
os.setgroups([65534]) # nogroup
os.setgid(65534) # nobody
os.setuid(65534) # nobody
os.seteuid(65534) # nobody
os.setegid(65534) # nobody
os.execve('./a.out', ['./a.out'], {})
