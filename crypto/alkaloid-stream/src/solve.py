#!/usr/bin/env python3

with open('output.txt', 'r') as f:
    enc = bytes.fromhex(f.readline().strip())
    public = eval(f.readline())

ln = len(public)

lkey, lidx = 0, -1
keys = []
bitstream = [None for _ in range(ln)]
for i in range(ln):
    for idx, vs in enumerate(public):
        if idx == lidx:
            continue

        if vs[0] == lkey:
            keys.append(vs[1])
            bitstream[idx] = 1
            lidx = idx
            break
        elif vs[1] == lkey:
            keys.append(vs[0])
            bitstream[idx] = 0
            lidx = idx
            break
    
    lkey ^= keys[-1]
    if len(keys) >= ln // 3 + 1:
        lkey ^= keys[-1 - ln // 3]

def xor(a, b):
    return [x ^ y for x, y in zip(a, b)]

def bytes_to_bits(inp):
    res = []
    for v in inp:
        res.extend(list(map(int, format(v, '08b'))))
    return res

def bits_to_bytes(inp):
    res = []
    for i in range(0, len(inp), 8):
        res.append(int(''.join(map(str, inp[i:i+8])), 2))
    return bytes(res)

print(bits_to_bytes(xor(bytes_to_bits(enc), bitstream)))