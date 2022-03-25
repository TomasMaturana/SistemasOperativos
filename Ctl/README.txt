Pregunta 1 parte a

La solucion debe ir en el archivo p1a.c.

Define la ubicacion de NSYSTEM con export NSYSTEM=.../psystem64-alfa2
Si, pruebe esta pregunta en pSystem porque es mas facil depurar.

Compile con: make test-p1a

Ejecute con:
make test-p1a-g  (version de debugging)
make test-p1a-O  (version optimizada)
make test-p1a-valgrind-ddd (para depurar errores de manejo de memoria)
make test-p1a-drd-ddd (para depurar dataraces)

Pregunta 1 parte b

La solucion debe ir en el archivo p1b.c.

Define la ubicacion de NSYSTEM con export NSYSTEM=.../psystem64-alfa2
Si, pruebe esta pregunta en pSystem porque es mas facil depurar.

Compile con: make test-p1b

Ejecute con:
make test-p1b-g  (version de debugging)
make test-p1b-O  (version optimizada)
make test-p1b-valgrind-ddd (para depurar errores de manejo de memoria)
make test-p1b-drd-ddd (para depurar dataraces)

Pregunta 2 parte i

La solucion debe ir en el archivo src/nPedir.c (en el directorio src)

Viene incluido el codigo fuente de nSystem beta3 en los directorios
src include y lib.  No necesita definir la varible NSYSTEM.

Compile con: make test-p2i

Ejecute con: ./test-p2i

Depure con: ddd test-p2i

No hay posibilidades de usar valgrind o drd.
