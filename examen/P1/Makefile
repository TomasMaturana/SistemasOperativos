# Compile el nSystem con:
# $ cd nsrc
# $ make

# Importante:
# Para compilar exitosamente su tarea necesita definir la variable de
# ambiente NSYSTEM con:
# $ export NSYSTEM=.../nsystem64-beta2
# (debe reemplazar ... por la ruta para llegar a nsystem64-beta2)

# Programe su tarea en el archivo subasta.c y luego pruebela con:
# $ cd <el directorio en donde se encuentra este archivo>
# $ make test-subasta
# $ ./test-subasta

# Programe su test de prueba personal en test2-subasta.c y pruebelo con:
# $ make test2-subasta
# $ ./test2-subasta

# Con opciones de debugging
CFLAGS= -Wall -pedantic -std=c99 -g -Iinclude -Isrc
LDFLAGS= -g
LDLIBS= lib/libnSys.a -lpthread

all: test-subasta

test-subasta: test-subasta.o subasta.o

test-subasta.o subasta.o: subasta.h

clean:
	rm -f *.o test-subasta
