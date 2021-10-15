# Fuck with uncompyle, which does not support unions of dicts (3.9 feature)
#“ㅤ”
@__import__('dataclasses').dataclass
class ㅤ:
    x: int
    y: str

ㅤㅤㅤ = bytes(e^1337 for e in [1385, 1403, 1402, 1389, 1407]) # b"PBCTF"
ㅤㅤㅤㅤㅤㅤ = getattr(__import__('hashlib'), 'md5')
I = input("flag? ") # "pbctf{dece0227383ca2ac793545ee989ce386}"
ㅤㅤㅤㅤ = lambda x: ㅤㅤㅤㅤㅤㅤ(x).hexdigest()
ㅤㅤㅤㅤㅤ = [list(I)]
ㅤㅤ = {}
# State variable for multiple matches
#   -1 = strip flag format
#   0 - 30 check flag
ㅤㅤㅤㅤㅤㅤㅤ = -1

while ㅤㅤㅤㅤㅤ:
    match ㅤㅤㅤㅤㅤ.pop(0):
        case ['p','b','c','t','f','{', *R, '}']:
            ㅤㅤ |= {0:112} if (ㅤㅤㅤㅤㅤㅤㅤ:=ㅤㅤㅤㅤㅤㅤㅤ+1) or len(R)!=32 else {}
            ㅤㅤㅤㅤㅤ=list(ㅤ(*e) for e in enumerate(R))
        case ㅤ(x,y):
            ㅤㅤ |= {x:y} if ((ㅤㅤㅤㅤㅤㅤㅤ:=ㅤㅤㅤㅤㅤㅤㅤ+1)!=x+1 or ㅤㅤㅤㅤ(ㅤㅤㅤ*x)[x]!=y) else {}
        case _:
            ㅤㅤ |= {1:125}
            break

print("Correct" if not ㅤㅤ else "Nope")