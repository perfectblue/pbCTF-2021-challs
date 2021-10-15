.PHONY: clean run

a.out: driver.cpp sha.cpp out.o
	clang++ -Oz driver.cpp sha.cpp out.o -lcurl

run: a.out remote/exp.enc
	./a.out

test: encrypt.cpp sha.cpp out.o
	clang++ -std=c++20 encrypt.cpp sha.cpp out.o -o test

remote/exp.enc: test remote/exp
	./test remote/exp

clean:
	rm -f a.out test remote/*.enc
