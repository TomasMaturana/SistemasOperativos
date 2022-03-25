#include <stdio.h>
#include <string.h>

#include <nSystem.h>

#include "equipo.h"

int jugador(char *nom, char ***pequipo) {
  char **equipo= hay_equipo(nom);
  int i= 0;

  while (i<5) {
    if (strcmp(equipo[i], nom)==0)
      break;
    i++;
  }

  if (i==5)
    nFatalError("jugador", "%s no esta en el equipo\n", nom);

  if (pequipo!=NULL)
    *pequipo= equipo;

  return 0;
}

#define N 10000
// Cuidado: M debe ser multiplo de 5
#define M 1000

static char **noms;
static int prox;
static nMonitor m;

int mult_jug() {

  nEnter(m);
  while (prox<N*M) {
    char *nom= noms[prox++];
    nExit(m);
    jugador(nom, NULL);
    nEnter(m);
  }
  nExit(m);
  return 0;
}

int nMain() {
  m= nMakeMonitor();
  init_equipo();
  {
    nPrintf("Test 1: el del enunciado\n");
    nTask jugadores[10];
    char *noms[]= {"pedro", "juan", "alberto", "enrique", "diego",
                    "jaime", "jorge", "andres", "jose", "luis"};
    char **equipos[10];
    for (int i=0; i<10; i++) {
      nSleep(100);
      nPrintf("lanzando %s\n", noms[i]);
      jugadores[i]= nEmitTask(jugador, noms[i], &equipos[i]);
    }
    for (int i=0; i<10; i++) {
      nWaitTask(jugadores[i]);
      nPrintf("terminado %s\n", noms[i]);
    }
    for (int i= 1; i<5; i++) {
      if (equipos[i]!=equipos[0])
        nFatalError("main", "%s no esta en el mismo equipo que %s\n",
                noms[i], noms[0]);
    }
    for (int i= 6; i<10; i++) {
      if (equipos[i]!=equipos[5])
        nFatalError("nMain", "%s no esta en el mismo equipo que %s\n",
                noms[i], noms[5]);
    }
    if (equipos[0]==equipos[5])
      nFatalError("nMain", "No pueden ser los mismos equipos\n");
 
    nFree(equipos[0]);
    nFree(equipos[5]);
    nPrintf("Test 1: aprobado\n");
  }

  nSetTimeSlice(1);

  {
    nPrintf("Test 2: busca dataraces (toma unos 30 segundos)\n");
    noms= nMalloc(N*M*sizeof(char*));
    nTask *jugadores= nMalloc(M*sizeof(nTask));
    for (int i= 0; i<N*M; i++) {
      char *nom= nMalloc(10);
      sprintf(nom, "j%08d", i);
      noms[i]= nom;
    }
    for (int i= 0; i<M; i++) {
      jugadores[i]= nEmitTask(mult_jug);
    }
    for (int i= 0; i<M; i++) {
      nWaitTask(jugadores[i]);
    }
    nPrintf("Test 2: aprobado\n");
  }
    
  nPrintf("Felicitaciones, su tarea paso todos los tests\n");
  return 0;
}
