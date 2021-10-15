#!/usr/bin/env sage

from Crypto.Cipher import AES
from Crypto.Util.number import *
from pwn import *
import sys

F.<x> = GF(2^128, name='x', modulus=x^128+x^7+x^2+x+1)

def bytes_to_element(b):
    v = bytes_to_long(b)
    e = 0
    for i in range(128):
        e += ((v >> (127 - i)) & 1) * x^i
    return e

def element_to_bytes(e):
    inv = e.integer_representation()
    v = int(f"{inv:0128b}"[::-1], 2)
    return long_to_bytes(v)

key = b"goodhashGOODHASH"
H = bytes_to_element(AES.new(key, AES.MODE_ECB).encrypt(b'\0' * 16))

if len(sys.argv) >= 3:
    r = remote(sys.argv[1], sys.argv[2])
else:
    r = process(['python3', 'main.py'])

r.recvuntil(b'Body: ')

nonce = r.recvline().strip()
fill = (16 - (len(nonce) % 16)) % 16 + 8
ghash_in = (nonce + b'\x00' * fill + long_to_bytes(8 * len(nonce), 8))

v = 0
num_block = len(ghash_in) // 16
for i in range(num_block):
    v += bytes_to_element(ghash_in[16*i:16*i+16]) * H^(num_block - i)

# Target:
# 0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef
# {"0123456789": "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", "admin":true}

tmp = b'{"0123456789": "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@", "admin":true}'
tmp += b'\x00' * 8 + long_to_bytes(8 * 64, 8)
for i in range(5):
    v += bytes_to_element(tmp[16*i:16*i+16]) * H^(5 - i)
v = v.integer_representation()

mat = [[] for _ in range(128)]
print(mat)
for i in range(16):
    for j in range(5):
        tmp = bytes([0] * i + [1 << j] + [0] * (15 - i))
        v1 = (bytes_to_element(tmp) * H^4).integer_representation()
        v2 = (bytes_to_element(tmp) * H^3).integer_representation()

        for k in range(128):
            mat[k].append((v1 >> k) & 1)
        for k in range(128):
            mat[k].append((v2 >> k) & 1)

target = []
for i in range(128):
    target.append((v >> i) & 1)

mat = Matrix(GF(2), mat)
target = vector(GF(2), target)
sol = mat.solve_right(target)

ans = bytearray(b'{"0123456789": "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@", "admin":true}')
for i in range(16):
    for j in range(5):
        if sol[10 * i + 2 * j]:
            ans[16 + i] |= 1 << j
        if sol[10 * i + 2 * j + 1]:
            ans[32 + i] |= 1 << j

# If \ in ans, please run again
assert b'\\' not in ans

print(ans)

r.sendlineafter(b'>', ans)
r.interactive()
