#!/usr/bin/env sage

from Crypto.Util.number import *

N = 144256630216944187431924086433849812983170198570608223980477643981288411926131676443308287340096924135462056948517281752227869929565308903867074862500573343002983355175153511114217974621808611898769986483079574834711126000758573854535492719555861644441486111787481991437034260519794550956436261351981910433997
e = 3707368479220744733571726540750753259445405727899482801808488969163282955043784626015661045208791445735104324971078077159704483273669299425140997283764223932182226369662807288034870448194924788578324330400316512624353654098480234449948104235411615925382583281250119023549314211844514770152528313431629816760072652712779256593182979385499602121142246388146708842518881888087812525877628088241817693653010042696818501996803568328076434256134092327939489753162277188254738521227525878768762350427661065365503303990620895441197813594863830379759714354078526269966835756517333300191015795169546996325254857519128137848289

P.<x, y> = PolynomialRing(ZZ)

m = 4
t = 2
X = int(N ^ 0.4)
Y = 3 * int(N ^ 0.5)

a = N + 1
b = N^2 - N + 1

f = x * (y^2 + a * y + b) + 1

gs = []

for k in range(m + 1):
    for i in range(k, m + 1):
        for j in range(2 * k, 2 * k + 2):
            g = x^(i - k) * y^(j - 2 * k) * f^k * e^(m - k)
            gs.append((i, j, k, g))
    i = k
    for j in range(2 * k + 2, 2 * k + t + 1):
        g = x^(i - k) * y^(j - 2 * k) * f^k * e^(m - k)    
        gs.append((i, j, k, g))

gs.sort()

monomials = []
for tup in gs:
    for v in tup[-1].monomials():
        if v not in monomials:
            monomials.append(v)

mat = [[0 for j in range(len(monomials))] for i in range(len(gs))]

for i, tup in enumerate(gs):
    for j, mono in enumerate(monomials):
        mat[i][j] = tup[-1].monomial_coefficient(mono) * mono(X, Y)

mat = Matrix(ZZ, mat)
mat = mat.LLL()

pols = []

for i in range(len(gs)):
    f = sum(mat[i, k] * monomials[k] / monomials[k](X, Y) for k in range(len(monomials)))
    pols.append(f)

found = False

for i in range(len(gs)):
    for j in range(i + 1, len(gs)):
        f1, f2 = pols[i], pols[j]

        rr = f1.resultant(f2)
        if rr.is_zero() or rr.monomials() == [1]:
            continue
        else:
            try:
                PR.<q> = PolynomialRing(ZZ)
                rr = rr(q, q)
                soly = int(rr.roots()[0][0])
                ss = f1(q, soly)
                solx = int(ss.roots()[0][0])

                print(i, j)
                print(solx, soly)
                assert f1(solx, soly) == 0
                assert f2(solx, soly) == 0

                found = True
            except:
                pass
        if found:
            break
    if found:
        break

b, c = soly, N
Dsqrt = int(sqrt(b^2 - 4*c))
p, q = (b + Dsqrt) // 2, (b - Dsqrt) // 2
assert p * q == N

phi = (p**2 + p + 1) * (q**2 + q + 1)
d = inverse(e, phi)

def add(P, Q, mod):
    m, n = P
    p, q = Q

    if p is None:
        return P
    if m is None:
        return Q
    
    if n is None and q is None:
        x = m * p % mod
        y = m + p % mod
        return (x, y)
    
    if n is None and q is not None:
        m, n, p, q = p, q, m, n
    
    if q is None:
        if (n + p) % mod != 0:
            x = (m * p + 2) * inverse(n + p, mod) % mod
            y = (m + n * p) * inverse(n + p, mod) % mod
            return (x, y)
        elif (m - n**2) % mod != 0:
            x = (m * p + 2) * inverse(m - n**2, mod) % mod
            return (x, None)
        else:
            return (None, None)
    else:
        if (m + p + n * q) % mod != 0:
            x = (m * p + (n + q) * 2) * inverse(m + p + n * q, mod) % mod
            y = (n * p + m * q + 2) * inverse(m + p + n * q, mod) % mod
            return (x, y)
        elif (n * p + m * q + 2) % mod != 0:
            x = (m * p + (n + q) * 2) * inverse(n * p + m * q + r, mod) % mod
            return (x, None)
        else:
            return (None, None)

def power(P, a, mod):
    res = (None, None)
    t = P
    while a > 0:
        if a % 2:
            res = add(res, t, mod)
        t = add(t, t, mod)
        a >>= 1
    return res

E = (123436198430194873732325455542939262925442894550254585187959633871500308906724541691939878155254576256828668497797665133666948295292931357138084736429120687210965244607624309318401630252879390876690703745923686523066858970889657405936739693579856446294147129278925763917931193355009144768735837045099705643710, 47541273409968525787215157367345217799670962322365266620205138560673682435124261201490399745911107194221278578548027762350505803895402642361588218984675152068555850664489960683700557733290322575811666851008831807845676036420822212108895321189197516787046785751929952668898176501871898974249100844515501819117)
M = power(E, d, N)

ln = 71
print(long_to_bytes(M[0])[: ln // 2] + long_to_bytes(M[1])[:ln - ln // 2])
