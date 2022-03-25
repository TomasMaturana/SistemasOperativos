# Para Linux/amd64
SYSDEP= nStack-amd64.o
MAKELIB= ar-ranlib
DEFINES=

# Para Linux/i386
# SYSDEP= nStack-i386.o
# MAKELIB= ar-ranlib
# DEFINES=
#
# Para Sparc/Solaris
# SYSDEP= nStack-sparc.o
# MAKELIB= ar-lorder
# DEFINES= -DREOPENSTDIO -DSOLARIS
#
# Para Sparc/SunOS 4.X.X:
# SYSDEP= nStack-sparc.o
# MAKELIB= ar-ranlib
# DEFINES= -DREOPENSTDIO -DNO_UNDERSCORE
# OBS. IMPORTANTE: el directorio /usr/ccs/bin debe ir antes que /bin
# /usr/bin y /usr/local/bin en el path de busqueda de comandos

#------ fin parte parte dependiente -----

NSYSTEMOBJS= nProcess.o nTime.o nMsg.o nSem.o nMonitor.o nIO.o nDep.o \
             nMain.o nQueue.o nOther.o fifoqueues.o nPedir.o $(SYSDEP)

LIBNSYS= ../lib/libnSys.a

CFLAGS= -g -Wall -pedantic -std=c99 -I../include $(DEFINES)
LDFLAGS= -g 

#CFLAGS= -O -Wall -pedantic -std=c99 -I../include $(DEFINES)
#LDFLAGS=

# Para Linux x86 en maquinas de 64 bits (amd64)
#CFLAGS= -g -m32 -Wall -pedantic -std=c99 -I../include $(DEFINES)
#LDFLAGS= -g -m32

all: libnsys

$(NSYSTEMOBJS): nQueue.h nSysimp.h ../include/fifoqueues.h ../include/nSystem.h

libnsys: $(NSYSTEMOBJS)
	sh $(MAKELIB) $(LIBNSYS) $(NSYSTEMOBJS)

debug: libnsys

optimize:
	make CFLAGS="-O -Wall -pedantic -std=c99 -I../include $(DEFINES)" LDFLAGS=""

clean:
	rm -f *.o *~
