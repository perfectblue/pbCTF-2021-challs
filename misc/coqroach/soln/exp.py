from pwn import *

cur_path = os.path.dirname(os.path.realpath(__file__))
soln = open(cur_path + '/SimplSoln.v', 'r').read()
tut = open(cur_path + '/TutorialSoln.v', 'r').read()
_, _, soln = soln.partition('(*** START SUBMISSION ***)')

p = remote('localhost', 1337)

def send_theorem(begin_marker, data=tut):
    print(str(p.recvuntil('#############################', drop=True), 'utf8'))
    _, _, data = data.partition(begin_marker)
    if 'BABY' in begin_marker:
        data, _, _ = data.partition('Qed.')
        end = 'Qed.'
    else:
        data, _, _ = data.partition('(* END *)')
        end = 'EOF'
    p.sendline(data)
    p.sendline(end)


p.recvuntil('(y/n)')

if input('Would you like to go through tutorials? (y/N) ')[0].lower() == 'y':
    p.sendline('n')
    tut = ['BABY'] * 4 + ['THEOREM'] * 4
    for i, t in enumerate(tut):
        send_theorem(f'(* {t} {i+1} *)')
else:
    p.sendline('y')

print(str(p.recvuntil('#############################', drop=True), 'utf8'))
p.recvuntil('EOF')
p.sendline(soln)
p.sendline('EOF')
p.interactive()
