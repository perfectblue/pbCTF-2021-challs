with open('output.txt', 'r') as f:
    enc = bytes.fromhex(f.readline().strip())
    public = eval(f.read())

ln = len(public)
S = []
for i in range(ln):
    for j in range(2):
        if public[i][j] != 0:
            S.append((i, j))
            break
    if S:
        break
        
while True:
    print(len(S))
    # Basis
    SB = [[public[i][j], 1 << k] for k, (i, j) in enumerate(S)]
    for i in range(len(SB)):
        lsb = SB[i][0] & -SB[i][0]
        for j in range(len(SB)):
            if i != j and SB[j][0] & lsb:
                SB[j][0] ^= SB[i][0]
                SB[j][1] ^= SB[i][1]

    print("Calculated basis")

    # For R -> L edges
    LS = dict()
    for i in range(len(S)):
        LS[S[i][0]] = i

    edges = [ [] for _ in range(len(SB)) ]
    Y1 = []
    Y2 = set()

    inputs = set([(i, j) for j in range(2) for i in range(ln)]) - set(S)
    for i, j in inputs:
        v, t = public[i][j], 0
        for k in range(len(SB)):
            lsb = SB[k][0] & -SB[k][0]
            if v & lsb:
                v ^= SB[k][0]
                t ^= SB[k][1]

        if v != 0:
            for k in range(len(SB)):
                edges[k].append((i, j))
            Y1.append((i, j))
            continue
        
        for k in range(len(SB)):
            if t & (1 << k):
                edges[k].append((i, j))
    
    print("Calculated all edges")

    for i in range(ln):
        for j in range(2):
            if i not in LS:
                Y2.add((i, j))
    
    print("Calculated Y2")

    check = set(Y1)
    tmp = check & Y2
    if tmp:
        # Intersection of Y1 and Y2 is not empty
        # Just use it
        S.append(tmp.pop())
        continue

    # BFS
    queue = [ [v] for v in Y1 ]
    apath = None
    while queue:
        path = queue.pop(0)
        v = path[-1]
        
        # R -> L edges
        if type(v) == tuple:
            if v[0] not in LS:
                for u in range(len(S)):
                    if u in check: continue
                    check.add(u)
                    queue.append(path + [u])
            else:
                if not LS[v[0]] in check:
                    check.add(LS[v[0]])
                    queue.append(path + [LS[v[0]]])
            continue
        
        # L -> R edges
        for u in edges[v]:
            if u in check: continue
            if u in Y2:
                apath = path + [u]
                break
            check.add(u)
            queue.append(path + [u])
        if apath:
            break
    
    print("BFS End")
    print(apath)
    if apath is None:
        break

    indices = set(apath[1::2])
    new_S = []
    for i in range(len(S)):
        if i in indices: continue
        new_S.append(S[i])
    S = new_S + apath[0::2]
    print(S)

    t = set(map(lambda x: x[0], S))
    assert len(t) == len(S), "Matroid on sets wrong"

ans = [0] * ln
for i, j in S:
    ans[i] = j

def bits_to_bytes(inp):
    res = []
    for i in range(0, len(inp), 8):
        res.append(int(''.join(map(str, inp[i:i+8])), 2))
    return bytes(res)

ans = bits_to_bytes(ans)
print(bytes([x ^ y for x, y in zip(ans, enc)]))
