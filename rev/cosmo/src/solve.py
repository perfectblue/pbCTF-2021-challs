from zlib import adler32
from itertools import product
from string import printable

flag = [0x014400d3,
0x042401aa,
0x08bf028b,
0x0efa034f,
0x16a1040d,
0x200004ea,
0x2ae20597,
0x3721065c,
0x4507072b,
0x542f07cd,
0x651208a2,
0x77860970,
0x8b8f0a34,
0xa0d50adf,
0xb75c0b75,
0xcfa40c5e,
0xe9440d01,
0x04520db2,
0x20b10e6e]

known = b""
for e in flag:
    for comb in product(printable, repeat=2):
        guess = ''.join(comb).encode()
        if adler32(known + guess) == e:
            known += guess
            break
    else:
        assert False

print(known)