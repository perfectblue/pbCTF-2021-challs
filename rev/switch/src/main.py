import py_compile, dis, marshal, shutil
py_compile.compile('challenge.py')
shutil.copy("__pycache__/challenge.cpython-310.pyc", "../dist/challenge.cpython-310.pyc")

print(dis.dis(marshal.loads(open("__pycache__/challenge.cpython-310.pyc","rb").read()[16:])))