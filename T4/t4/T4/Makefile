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

all: test-t4

test-t4: test.o t4.o
	gcc $(LDFLAGS) test.o t4.o $(LDLIBS) -o test-t4

run-g:
	make clean
	make
	bash -c 'time ./test-t4'

run-O:
	make clean
	make CFLAGS="-O -Wall -pedantic -std=c99 -I$(NSYSTEM)/include" LDFLAGS=""
	bash -c 'time ./test-t4'

run-valgrind-ddd:
	make clean
	make CFLAGS="-g -DVALGRIND=1 -Wall -pedantic -std=c99 -I$(NSYSTEM)/include"
	bash -c 'valgrind --leak-check=full --vgdb=yes --vgdb-error=0 ./test-t4 |& tee run.log'

run-drd-ddd:
	make clean
	make CFLAGS="-g -DVALGRIND=1 -Wall -pedantic -std=c99 -I$(NSYSTEM)/include"
	bash -c 'valgrind --tool=drd --vgdb=yes --vgdb-error=0 ./test-t4 |& tee run.log'

clean:
	rm -f *.o test-t4
