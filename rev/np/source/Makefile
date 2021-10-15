CPPFLAGS= -MMD -MP
LDFLAGS=-pthread
sources := np.cc ast.cc calc.cc base64.cc
objects := $(sources:%.cc=obj/%.o)
dev_objects := $(sources:%.cc=obj/%.dev.o)
deps    := $(objects:.o=.d)

all: np.dev np lockfile

check: np lockfile
	(cat flag && cat nonce) | ./np lockfile

lockfile: np.dev
	./$< $@ > nonce

np: $(objects)
	$(LINK.cc) $(OUTPUT_OPTION) $^

np.dev: $(dev_objects)
	$(LINK.cc) $(OUTPUT_OPTION) $^

obj/%.dev.o: %.cc
	@mkdir -p obj
	$(COMPILE.cc) -g -DDEBUG $(OUTPUT_OPTION) $<

obj/%.o: %.cc
	@mkdir -p obj
	$(COMPILE.cc) $(OUTPUT_OPTION) $<

np.o: ast.hh calc.hh base64.hh
ast.o: ast.hh

coq: Makefile.coq
	make -f Makefile.coq all

clean:
	(test -f Makefile.coq && make -f Makefile.coq clean) || true
	rm -f np np.dev Makefile.coq.conf nonce lockfile Makefile.coq

Makefile.coq: _CoqProject Makefile
	coq_makefile -f _CoqProject -o $@

_CoqProject: ;

Makefile: ;

phony: ;

.PHONY: all clean phony coq

-include $(deps)
