#include <string.h>
#include <nSystem.h>

#include "spinlock.h"
#include "pedir.h"

#ifndef VALGRIND

#define NTASKS 32
#define ITER 20000
#define OCUP 100

#else

#define NTASKS 32
#define ITER 2000
#define OCUP 100

#endif

// Manejo de pausas

static int tiempo_actual= 0;
nMonitor t_mon;

static int tiempoActual() {
  nEnter(t_mon);
  int t= tiempo_actual;
  nExit(t_mon);
  return t;
}

static void pausa(int tiempo_espera) {
  nEnter(t_mon);
  int tiempo_inicio= tiempo_actual;
  nExit(t_mon);
  nSleep(tiempo_espera*1000);
  nEnter(t_mon);
  tiempo_actual= tiempo_inicio+tiempo_espera;
  nExit(t_mon);
}

// Indentacion

static char *espacios="                                                  ";

static char *indent(int t) {
  int len= strlen(espacios);
  return &espacios[len-(t-1)*10];
}

// Manejo de coreId

static int coreIdDist= 0;
static __thread int thisCoreId;

static void resetCoreId() {
  coreIdDist= 0;
}

static void setCoreId() {
  nEnter(t_mon);
  thisCoreId= coreIdDist++;
  nExit(t_mon);
}

int coreId() {
  return thisCoreId;
}

// Para el test del enunciado

static int th_fun(int tpedir, int num_th, int cat, int tadq, int tocup) {
  setCoreId();
  char *ind= indent(num_th);
  nPrintf("Tpo=%d:%s T%d (core %d) invoca pedir(%d)\n",
          tiempoActual(), ind, num_th, coreId(), cat);
  if (cat>1 || cat<0)
    nFatalError("test", "La categoria %d es incorrecta\n", cat);
  pedir(cat);
  nPrintf("Tpo=%d:%s T%d cat %d obtiene recurso\n",
          tiempoActual(), ind, num_th, cat);
  if (tadq!=tiempoActual()) {
    nFatalError("test", "T%d obtiene recurso en tiempo erroneo\n", num_th);
  }
  pausa(tocup);
  nPrintf("Tpo=%d:%s T%d devuelve recurso\n", tiempoActual(), ind, num_th);
  devolver();
  return 0;
}

// Los spinLocks
// No son verdaderos spinLocks, pero son funcionalmente equivalentes

void initSpinLock(SpinLock *psl, int ini) {
  psl->busy= ini;
  psl->m= nMakeMonitor();
}

void destroySpinLock(SpinLock *psl) {
  nDestroyMonitor(psl->m);
}
  
void spinLock(SpinLock *psl) {
  nEnter(psl->m);
  while (psl->busy==1)
    nWait(psl->m);
  psl->busy= 1;
  nExit(psl->m);
}

void spinUnlock(SpinLock *psl) {
  nEnter(psl->m);
  psl->busy= 0;
  nNotifyAll(psl->m);
  nExit(psl->m);
}

// Para el test de robustez

static int dentro= 0;
static int cnt= 0;

static int rob_fun(int cat, int n) {
  setCoreId();
  nPrintf("c%d", coreId());
  for (int k= 0; k<n; k++) {
    int punto= 0;
    pedir(cat);
    if (dentro==1)
      nFatalError("test", "No se cumple la exclusion mutua\n");
    dentro= 1;
    cnt++;
    if (cnt%10000==0)
      punto= 1;
    for (int i=0; i<OCUP; i++)
      ;
    dentro= 0;
    devolver();
    if (punto)
      nPrintf(".");
  }
  return 0;
}

int nMain() {
  t_mon= nMakeMonitor();

  iniciar();

  setCoreId();
  nPrintf("El ejemplo del enunciado\n");
  nPrintf("------------------------\n\n");
  // Parametros de th_fun: tiempo de invocacion de pedir, num. thread,
  // categoria, tiempo en que se espera que obtenga recurso,
  // tiempo que toma en devolver el recurso
  nTask t1= nEmitTask(th_fun, 0, 1, 0, 0, 4);
  pausa(1);
  nTask t3= nEmitTask(th_fun, 1, 3, 0, 6, 1);
  pausa(1);
  nTask t2= nEmitTask(th_fun, 2, 2, 1, 4, 2);
  pausa(1);
  nTask t5= nEmitTask(th_fun, 3, 5, 0, 8, 1);
  nPrintf("Tpo=%d:%s Esperando que T1 termine\n", tiempoActual(), indent(1));
  nWaitTask(t1);
  nPrintf("Tpo=%d:%s T1 termino\n", tiempoActual(), indent(1));
  pausa(1);
  nTask t4= nEmitTask(th_fun, 5, 4, 1, 7, 1);
  pausa(1);
  nPrintf("Tpo=%d:%s Esperando que T2 termine\n", tiempoActual(), indent(2));
  nWaitTask(t2);
  nPrintf("Tpo=%d:%s T2 termino\n", tiempoActual(), indent(2));
  nPrintf("Tpo=%d:%s Esperando que T3 termine\n", tiempoActual(), indent(3));
  nWaitTask(t3);
  nPrintf("Tpo=%d:%s T3 termino\n", tiempoActual(), indent(3));
  nPrintf("Tpo=%d:%s Esperando que T4 termine\n", tiempoActual(), indent(4));
  nWaitTask(t4);
  nPrintf("Tpo=%d:%s T4 termino\n", tiempoActual(), indent(4));
  nPrintf("Tpo=%d:%s Esperando que T5 termine\n", tiempoActual(), indent(5));
  nWaitTask(t5);
  nPrintf("Tpo=%d:%s T5 termino\n", tiempoActual(), indent(5));

  resetCoreId();
  setCoreId();

  nPrintf("\nTest de robustez\n");
  nPrintf("----------------\n\n");
  nPrintf("\ndespliega unos 70 puntos\n");
  nPrintf("c1 c2 c3 ... significa que ese core comenzo a trabajar\n\n");
  nSetTimeSlice(1);
  long long mask= 0xf315cd57;
  nTask *tasks= nMalloc(NTASKS*sizeof(nTask));
  for (int i= 0; i<NTASKS; i++)
    tasks[i]= nEmitTask(rob_fun, (mask>>(i&0x3f)) & 1, ITER);
  for (int i= 0; i<NTASKS; i++)
    nWaitTask(tasks[i]);
  
  nFree(tasks);

  terminar();

  nPrintf("\nFelicitaciones: funciona\n");
  return 0;
}
