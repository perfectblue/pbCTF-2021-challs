#!/usr/bin/env python3

import random
from heapq import *
from keystone import *

ks = Ks(KS_ARCH_X86, KS_MODE_64)
width = 4
length = 800


def gen_graph():
    # Two edges: i -> edges[i][0], i -> edges[i][1]
    edges = [(2 * i + 1, 2 * i + 2) for i in range(2 ** width - 1)]
    cnt = 2 ** (width + 1) - 1

    for i in range(length - width - 1):
        f = list(range(2 ** width))
        s = []
        random.shuffle(f)

        for j in range(2 ** width):
            v = random.randint(0, 2 ** width - 2)
            if v >= f[j]:
                v += 1
            s.append(v)

        for j in range(2 ** width):
            edges.append((cnt + f[j], cnt + s[j]))
        cnt += 2 ** width

    for j in range(2 ** width):
        edges.append((cnt, cnt))

    sbox = list(range(1, cnt))
    random.shuffle(sbox)
    sbox = [0] + sbox + [cnt]

    edges_with_weight = [None for _ in range(cnt)]
    for i, (v1, v2) in enumerate(edges):
        w1 = random.randint(1, 100)
        w2 = random.randint(1, 100)
        if w2 >= w1:
            w2 += 1
        edges_with_weight[sbox[i]] = [(sbox[v1], w1), (sbox[v2], w2)]

    return edges_with_weight


def dijkstra(graph):
    cnt = len(graph)
    weights = [[0, [0]]] + [[2 ** 32, []] for _ in range(cnt)]
    check = [False for _ in range(cnt + 1)]
    pq = []
    heappush(pq, (0, 0))
    while pq:
        dist, idx = heappop(pq)
        if idx == cnt or check[idx]:
            continue
        check[idx] = True
        for v, w in graph[idx]:
            if dist + w == weights[v][0]:
                weights[v][1] = None
            if dist + w < weights[v][0]:
                weights[v][0] = dist + w
                if weights[idx][1] is None:
                    weights[v][1] = None
                else:
                    weights[v][1] = weights[idx][1] + [v]
                heappush(pq, (weights[v][0], v))

    return weights[-1]


def modify_graph_as_flag(graph, path, flag_bin):
    for i in range(len(path) - 1):
        p, v = path[i], flag_bin[i]
        if graph[p][v][0] != path[i + 1] or (
            graph[p][1 - v][0] == path[i + 1] and graph[p][v][1] > graph[p][1 - v][1]
        ):
            a, b = graph[p]
            graph[p] = [b, a]


base1 = """func_start:
    lea rax, [rip]
    mov r8, [rbx]
    xor [rax], r8
    mov r8, [rbx + 8]
    xor [rax + 8], r8
    mov r8, [rbx + 16]
    xor [rax + 16], r8
    mov r8, [rbx + 24]
    xor [rax + 24], r8
    mov al, [rsi]
    add rsi, 1
    test al, al
"""
assert len(ks.asm(base1)[0]) == 45

base2 = """jz false_case
    lea rbx, [rdi + {}]
    add r9, {}
    jmp func_start
false_case:
    lea rbx, [rdi + {}]
    add r9, {}
    jmp func_start"""

end = """    mov rax, 1
    mov rdi, 1
    mov rdx, 5
    cmp r9, {}
    jg false_case
    mov rsi, r10
    syscall
    jmp f_end
false_case:
    mov rsi, r11
    syscall
f_end:
    mov rax, 60
    xor rdi, rdi
    syscall"""


def xor(a, b):
    return bytes([x ^ y for x, y in zip(a, b)])


def gen_code(graph, weight):
    code_nodes = []
    for idx, edge in enumerate(graph):
        (_, w1), (_, w2) = edge

        code = base1
        code += base2.format(64 + 64 * idx, w2, 32 + 64 * idx, w1)
        code, _ = ks.asm(code)

        num_nops = 32 - (len(code) - 45)

        l = [0] * num_nops + [1] * 8
        random.shuffle(l)

        code = base2.format(64 + 64 * idx, w2, 32 + 64 * idx, w1)
        lines = code.split("\n")

        code = ""
        cnt = 0
        for v in l:
            if v:
                code += lines[cnt] + "\n"
                cnt += 1
            else:
                code += "nop\n"

        code = base1 + code
        code, _ = ks.asm(code)

        assert len(code) == 45 + 32
        code_nodes.append(bytes(code[45:]))

    code, _ = ks.asm(end.format(weight))
    code_nodes.append(bytes(code))

    code = xor(code_nodes[0], code_nodes[-1])
    for idx, edge in enumerate(graph):
        (v1, _), (v2, _) = edge
        code += xor(code_nodes[idx], code_nodes[v1])
        code += xor(code_nodes[idx], code_nodes[v2])

    return code, code_nodes[-1]


while True:
    graph = gen_graph()
    weight, path = dijkstra(graph)
    if path:
        break

assert len(path) == length + 1

with open("flag.txt", "rb") as f:
    flag = f.read()

flag_bin = [int(c) for c in "".join(f"{v:08b}"[::-1] for v in flag)]
assert len(flag_bin) == length
modify_graph_as_flag(graph, path, flag_bin)
code, last = gen_code(graph, weight)

with open("last.bin", "wb") as f:
    f.write(last)

with open("code.bin", "wb") as f:
    f.write(code)
