LIBNSYS=$(NSYSTEM)/lib/libnSys.a

# Con opciones de debugging
CFLAGS= -Wall -pedantic -std=c99 -g -I$(NSYSTEM)/include
LDFLAGS= -g
LDLIBS= $(LIBNSYS) -lpthread

# Para probar con pSystem (nSystem implementado a partir de pthreads)
# LDLIBS= $(LIBNSYS) -lpthread

# Con opciones de optimizacion:
# CFLAGS= -O2 -I$(NSYSTEM)/include -I$(NSYSTEM)/src
# LDFLAGS=

all: test-p1a test-p1b

test-p1a: p1a.o test.o
	gcc $(LDFLAGS) p1a.o test.o $(LDLIBS) -o test-p1a

run-p1a-g:
	make clean
	make test-p1a
	bash -c 'time ./test-p1a'

run-p1a-O:
	make clean
	make CFLAGS="-O -Wall -pedantic -std=c99 -I$(NSYSTEM)/include" LDFLAGS="" test-p1a
	bash -c 'time ./test-p1a'

run-p1a-valgrind-ddd:
	make clean
	make CFLAGS="-g -DVALGRIND=1 -Wall -pedantic -std=c99 -I$(NSYSTEM)/include" test-p1a
	bash -c 'valgrind --vgdb=yes --vgdb-error=0 ./test-p1a |& tee run.log'

run-p1a-drd-ddd:
	make clean
	make CFLAGS="-g -DVALGRIND=1 -Wall -pedantic -std=c99 -I$(NSYSTEM)/include" test-p1a
	bash -c 'valgrind --tool=drd --vgdb=yes --vgdb-error=0 ./test-p1a |& tee run.log'

test-p1b: p1b.o test.o
	gcc $(LDFLAGS) p1b.o test.o $(LDLIBS) -o test-p1b

run-p1b-g:
	make clean
	make test-p1b
	bash -c 'time ./test-p1b'

run-p1b-O:
	make clean
	make CFLAGS="-O -Wall -pedantic -std=c99 -I$(NSYSTEM)/include" LDFLAGS="" test-p1b
	bash -c 'time ./test-p1b'

run-p1b-valgrind-ddd:
	make clean
	make CFLAGS="-g -DVALGRIND=1 -Wall -pedantic -std=c99 -I$(NSYSTEM)/include" test-p1b
	bash -c 'valgrind --leak-check=full --vgdb=yes --vgdb-error=0 ./test-p1b |& tee run.log'

run-p1b-drd-ddd:
	make clean
	make CFLAGS="-g -DVALGRIND=1 -Wall -pedantic -std=c99 -I$(NSYSTEM)/include" test-p1b
	bash -c 'valgrind --tool=drd --vgdb=yes --vgdb-error=0 ./test-p1b |& tee run.log'

test-p2i:
	( cd src; make )
	gcc -g -Wall -pedantic -Iinclude -std=c99 -DnPEDIR test.c -o test-p2i lib/libnSys.a

clean:
	rm -f *.o test-p1a test-p1b test-p2i
