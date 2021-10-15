#!/usr/bin/env sage

from Crypto.Util.number import *
import itertools
import random
from gen import PRNG

outputs = []
with open('output.txt', 'r') as f:
    line = f.readline()
    for i in range(12):
        outputs.append(int(line[16 * i:16 * i + 16], 16))

    enc = bytes.fromhex(f.readline().strip())

m1 = 2 ** 32 - 107
m2 = 2 ** 32 - 5
m3 = 2 ** 32 - 209
M = 2 ** 64 - 59

rnd = random.Random(b'rbtree')
a1 = [rnd.getrandbits(20) for _ in range(3)]
a2 = [rnd.getrandbits(20) for _ in range(3)]
a3 = [rnd.getrandbits(20) for _ in range(3)]

A = crt([a1[2], a2[2], a3[2]], [m1, m2, m3])
B = crt([a1[1], a2[1], a3[1]], [m1, m2, m3])
C = crt([a1[0], a2[0], a3[0]], [m1, m2, m3])

RANGE = 8

for x in itertools.product(range(-1, 3), repeat=RANGE):
    print(x)

    o = [outputs[i] - x[i] * M for i in range(RANGE)]
    k = [inverse(-2 * m1 * m1, m2 * m3) * o[i] % (m2 * m3) for i in range(RANGE)]

    mat = [[0 for j in range(2 * RANGE - 2)] for i in range(2 * RANGE - 2)]

    for i in range(RANGE):
        mat[i][i] = 1
    for i in range(RANGE - 3):
        mat[i][RANGE + i] = -C
        mat[i + 1][RANGE + i] = -B
        mat[i + 2][RANGE + i] = -A
        mat[i + 3][RANGE + i] = 1
        mat[RANGE + i][RANGE + i] = m1 * m2 * m3
        mat[-1][RANGE + i] = (k[i + 3] - A * k[i + 2] - B * k[i + 1] - C * k[i]) * m1 % (m1 * m2 * m3)
    
    T = 2 ** 33
    mat[-1][-1] = T

    mat = Matrix(mat)
    mat = mat.LLL()

    for i in range(2 * RANGE - 2):
        flag = True
        for j in range(RANGE - 4):
            if mat[i][RANGE + j] != 0:
                flag = False
                break
        if flag and (mat[i][-1] == T or mat[i][-1] == -T):
            x_recov = list(map(int, mat[i][:RANGE]))
            x_recov[-1] -= mat[i][-2]
            print(x_recov)

            y_recov = []
            for i in range(RANGE):
                y_recov.append(int((2 * m1 * x_recov[i] - o[i]) * inverse(m3, m2) % m2))
            print(y_recov)

            z_recov = []
            for i in range(RANGE):
                z_recov.append(int((2 * m1 * x_recov[i] - o[i] - m3 * y_recov[i]) // m2))
            print(z_recov)

            prng = PRNG()
            prng.x = x_recov[:3]
            prng.y = y_recov[:3]
            prng.z = z_recov[:3]

            flag = True
            for i in range(12):
                if outputs[i] != int(prng.out().hex(), 16):
                    flag = False
                    break
            
            if flag:
                stream = b''
                for i in range(len(enc) // 8):
                    stream += prng.out()
                print(bytes([x ^^ y for x, y in zip(enc, stream)]))
                exit(0)


