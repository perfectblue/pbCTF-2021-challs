import struct

flag = b'pbctf{are_y0u_h4ving_fun_yet?!_im_lmaoing_at_ur_life_rn_KPd18iYszPNEA5rdAhF0G3NNpZXl1YSk}__'

def bitstream():
    for c in flag:
        x = bin(c)
        assert x.startswith('0b')
        x = x[2:].zfill(8)
        for c in x:
            assert c in '01'
            yield int(c)

result = None
from collections import deque
bits = deque(bitstream())

bits_orig = ''.join(map(str,bits))
print(''.join(map(str,bits)))

def grab(n):
    x = 0
    asdf = [bits.popleft() for _ in range(n)][::-1]
    for b in asdf:
        x |= b
        x = x << 1
    # print(x>>1)
    return x >> 1

import json

while len(bits) >= 15:
    # print(''.join(map(str,bits)))
    n = grab(2)
    if n == 0b00: # number
        n = grab(12)
        obj = n
    elif n == 0b01: # Null
        obj = None
    elif n == 0b10: # bool
        n = grab(1)
        if n == 1:
            obj = True
        else:
            obj = False
    elif n == 0b11: # string
        n = grab(6)
        obj = chr(n+0x20)

    n = grab(1)
    if n == 0:
        result = [result, obj]
    elif n == 1:
        result = { json.dumps(obj): result }
print('out of bits')

print(result)
print()
print(json.dumps(result))
print()

globalSpooky = 0xabc
def extremely_spooky_function(i):
    global globalSpooky
    globalSpooky += 0x123
    val = (i + globalSpooky) & 0xfff
    return f'ExtremelySpookyFunction({val})'

def extremely_spooky_function_2(b):
    if b == True:
        return f'ExtremelySpookyFunction2(false)'
    else:
        return f'ExtremelySpookyFunction2(true)'

# assert result == [[[{'true': {'false': [{'865': {'1445': [{'"3"': [[[{'720': [{'2475': [[{'2504': [[[{'2971': {'true': [[{'null': [{'null': [[[[{'null': {'false': {'true': {'false': [{'true': [{'","': [{'"M"': {'null': {'"D"': [{'"N"': {'null': {'744': {'true': {'"7"': {'false': {'false': {'null': {'null': {'false': [[{'"M"': {'null': {'":"': [[{'1405': [{'"0"': {'"*"': [[{'null': {'false': {'true': {'3306': {'true': {'"7"': {'false': {'"<"': [[{'false': {'1678': [{'3755': [{'false': [{'3243': {'false': {'true': [[[[[{'null': None}, False], 3148], '='], 3279], '8']}}}, '>']}, False]}, ':']}}, True], None]}}}}}}}}, 'O'], None]}}, 'Y']}, None], None]}}}, 1750], 2939]}}}}}}}}}}, None]}}}, 1686]}, None]}, 'N']}}}}, 'I'], '4'], 3206], None]}, False]}, None], None]}}, True], None], False]}, None], False]}, 2449]}, 1584], False], 'F']}, 2510]}}, 1381]}}, None], '?'], None]

bits =''
while result is not None:
    if isinstance(result, list):
        print('require(result.is_array());')
        print('require(result.size() == 2);')
        print('obj = result[1];')
        print('result = result[0];')
        obj = result[1]
        result = result[0]
        bits = '0' + bits
    elif isinstance(result, dict):
        print('require(result.is_object());')
        print('require(result.size() == 1);')
        print('obj = nlohmann::json::parse(result.items().begin().key());')
        print('result = result.items().begin().value();')
        obj = json.loads(next(iter(result.keys())))
        result = next(iter(result.values()))
        bits = '1' + bits
    if type(obj) == int:
        print('require(obj.is_number_integer());')
        print(f'require(obj.get<int>() == {extremely_spooky_function(obj)});')
        bits = '00' + bin(obj)[2:].zfill(12)[::-1] + bits
    elif obj is None:
        print('require(obj.is_null());')
        bits = '10' + bits
    elif type(obj) == bool:
        print('require(obj.is_boolean());')
        if obj:
            print(f'require(obj.get<bool>() == {extremely_spooky_function_2(True)});')
            bits = '011' + bits
        else:
            print(f'require(obj.get<bool>() == {extremely_spooky_function_2(False)});')
            bits = '010' + bits
    elif type(obj) == str:
        print('require(obj.is_string());')
        print(f'require(obj.get<std::string>()[0] == ' + repr(obj) + ');')
        n = ord(obj[0])-0x20
        bits = '11' + (bin(n)[2:].zfill(6))[::-1] + bits
    # print(bits)
# print(bits)
