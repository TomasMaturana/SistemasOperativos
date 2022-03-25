#include <string.h>
// Para poder usar rand
#include <stdlib.h>
#include <nSystem.h>

#include "subasta.h"

// Manejo de coreId

static nMonitor t_mon;
static int coreIdDist= 0;
static __thread int thisCoreId;

#if 0
static void resetCoreId() {
  coreIdDist= 0;
}
#endif

static void setCoreId() {
  nEnter(t_mon);
  thisCoreId= coreIdDist++;
  nExit(t_mon);
}

int coreId() {
  return thisCoreId;
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

nSem mutex;

// print_msg:
//   1   => con mensajes y delay
//   0   => sin mensajes, con delay
//   -1  => sin mensajes, sin delay

int aleatorio(Subasta s, int print_msg, char *nom, int oferta) {
  if (print_msg>=0) {
    nSetTaskName(nom);
    nWaitSem(mutex); 
    int delay= rand()%1000;
    nSignalSem(mutex);
    nSleep(delay);
  }
  if (print_msg>0)
    nPrintf("%s ofrece %d\n", nom, oferta);
  int ret= ofrecer(s, oferta);
  if (print_msg>0) {
    if (ret)
      nPrintf("%s se adjudico un item a %d\n", nom, oferta);
    else
      nPrintf("%s fallo con su oferta de %d\n", nom, oferta);
  }
  return ret;
}

int oferente(Subasta s, int print_msg, char *nom, int oferta) {
  nSetTaskName(nom);
  if (print_msg>0)
    nPrintf("%s ofrece %d\n", nom, oferta);
  int ret= ofrecer(s, oferta);
  if (print_msg>0) {
    if (ret)
      nPrintf("%s se adjudico un item a %d\n", nom, oferta);
    else
      nPrintf("%s fallo con su oferta de %d\n", nom, oferta);
  }
  return ret;
}

int test1(int print_msg) {
  Subasta s= nuevaSubasta(2);
  nTask pedro= nEmitTask(aleatorio, s, print_msg, "pedro", 1);
  nTask juan= nEmitTask(aleatorio, s, print_msg, "juan", 3);
  nTask diego= nEmitTask(aleatorio, s, print_msg, "diego", 4);
  nTask pepe= nEmitTask(aleatorio, s, print_msg, "pepe", 2);
  if (nWaitTask(pedro))
    nFatalError("test1", "pedro debio perder con 1\n");
  if (nWaitTask(pepe))
    nFatalError("test1", "pepe debio perder con 2\n");
  int u;
  nSleep(2000);
  int recaud= adjudicar(s, &u);
  if (recaud!=7)
    nFatalError("test1", "La recaudacion debio ser 7 y no %d\n", recaud);
  if (u!=2)
    nFatalError("test1", "Quedaron %d unidades sin vender\n", u);
  if (!nWaitTask(juan))
    nFatalError("nMain", "juan debio ganar con 6\n");
  if (!nWaitTask(diego))
    nFatalError("nMain", "diego debio perder con 4\n");
  if (print_msg>0)
    nPrintf("El monto recaudado es %d y quedaron %d unidades sin vender\n",
            recaud, u);
  return 0;
}

int test2(int print_msg) {
  Subasta s= nuevaSubasta(3);
  nTask ana= nEmitTask(oferente, s, print_msg, "ana", 7);
  if (print_msg>=0)
    nSleep(1000);
  nTask maria= nEmitTask(oferente, s, print_msg, "maria", 3);
  if (print_msg>=0)
    nSleep(1000);
  nTask ximena= nEmitTask(oferente, s, print_msg, "ximena", 4);
  if (print_msg>=0)
    nSleep(1000);
  nTask erika= nEmitTask(oferente, s, print_msg, "erika", 5);
  if (print_msg>=0)
    nSleep(1000);
  if (nWaitTask(maria))
    nFatalError("nMain", "maria debio perder con 3\n");
  nTask sonia= nEmitTask(oferente, s, print_msg, "sonia", 6);
  if (print_msg>=0)
    nSleep(1000);
  if (nWaitTask(ximena))
    nFatalError("nMain", "ximena debio perder con 4\n");
  int u;
  nSleep(2000);
  int recaud= adjudicar(s, &u);
  if (recaud!=18)
    nFatalError("test1", "La recaudacion debio ser 7 y no %d\n", recaud);
  if (u!=3)
    nFatalError("test1", "Quedaron %d unidades sin vender\n", u);
  if (!nWaitTask(ana))
    nFatalError("nMain", "ana debio ganar con 7\n");
  if (!nWaitTask(erika))
    nFatalError("nMain", "erika debio ganar con 5\n");
  if (!nWaitTask(sonia))
    nFatalError("nMain", "sonia debio ganar con 6\n");
  if (print_msg>0)
    nPrintf("El monto recaudado es %d y quedaron %d unidades sin vender\n",
            recaud, u);
  return 0;
}

int test3(int print_msg) {
  Subasta s= nuevaSubasta(5);
  nTask tomas= nEmitTask(oferente, s, print_msg, "tomas", 2, -1);
  if (print_msg>=0)
    nSleep(1000);
  nTask monica= nEmitTask(oferente, s, print_msg, "monica", 3, -1);
  if (print_msg>=0)
    nSleep(1000);
  int u;
  int recaud= adjudicar(s, &u);
  if (recaud!=5)
    nFatalError("test3", "La recaudacion debio ser 5 y no %d\n", recaud);
  if (u!=2)
    nFatalError("test3", "Quedaron %d unidades sin vender\n", u);
  if (print_msg>0)
    nPrintf("El monto recaudado es %d y quedaron %d unidades sin vender\n",
            recaud, u);
  if (!nWaitTask(tomas))
    nFatalError("nMain", "tomas debio ganar con 2\n");
  if (!nWaitTask(monica))
    nFatalError("nMain", "monica debio ganar con 3\n");
  return 0;
}

#define N 30
#define M 200

int nMain() {
  t_mon= nMakeMonitor();
  setCoreId();

  mutex= nMakeSem(1);
  nPrintf("una sola subasta con tiempos aleatorios\n");
  test1(1);
  nPrintf("test aprobado\n");
  nPrintf("-------------\n");

  nPrintf("una sola subasta con tiempos deterministicos\n");
  test2(1);
  nPrintf("test aprobado\n");
  nPrintf("-------------\n");

  nPrintf("una sola subasta con menos oferentes que unidades disponibles\n");
  test3(1);
  nPrintf("test aprobado\n");
  nPrintf("-------------\n");

  nSetTimeSlice(1);
  nPrintf("%d subastas en paralelo\n", N);
  nTask *tasks1= nMalloc(M*sizeof(nTask));
  nTask *tasks2= nMalloc(M*sizeof(nTask));
  nTask *tasks3= nMalloc(M*sizeof(nTask));
  if (tasks1==NULL || tasks2==NULL || tasks3==NULL)
    nFatalError("nMain", "Se acabo la memoria\n");
  int k;
  for (k=1; k<N; k++) {
    tasks1[k]= nEmitTask(test1, 0);
    tasks2[k]= nEmitTask(test2, 0);
    tasks3[k]= nEmitTask(test3, 0);
  }
  tasks1[0]= nEmitTask(test1, 1);
  tasks2[0]= nEmitTask(test2, 1);
  tasks3[0]= nEmitTask(test3, 1);
  for (k=0; k<N; k++) {
    nWaitTask(tasks1[k]);
    nWaitTask(tasks2[k]);
    nWaitTask(tasks3[k]);
  }
  nPrintf("test aprobado\n");
  nPrintf("-------------\n");
  nPrintf("%d subastas en paralelo\n", M*2);
  nPrintf("(tomo alrededor de un minuto en mi computador)\n");
  for (k=1; k<M; k++) {
    tasks1[k]= nEmitTask(test1, -1);
    tasks2[k]= nEmitTask(test2, -1);
  }
  tasks1[0]= nEmitTask(test1, 1);
  tasks2[0]= nEmitTask(test2, 1);
  nWaitTask(tasks1[0]);
  nWaitTask(tasks2[0]);
  nPrintf("Enterrando tareas.  Cada '.' son 30 tareas enterradas.\n");
  for (k=1; k<M; k++) {
    nWaitTask(tasks1[k]);
    nWaitTask(tasks2[k]);
    if (k%10==0) nPrintf(".");
  }
  nPrintf("\ntest aprobado\n");
  nPrintf(  "-------------\n");
  nPrintf("Felicitaciones: paso todos los tests\n");
  return 0;
}

// Implementacion de la cola de prioridades

#define MAXQSZ 100

struct priqueue {
  void **vec;
  int *ofertas;
  int size, maxsize;
};

PriQueue MakePriQueue(int maxsize) {
  PriQueue pq= nMalloc(sizeof(*pq));
  pq->maxsize= maxsize;
  pq->vec= nMalloc(sizeof(void*)*(maxsize+1));
  pq->ofertas= nMalloc(sizeof(int)*(maxsize+1));
  pq->size= 0;
  return pq;
}

void *PriGet(PriQueue pq) {
  void *t;
  int k;
  if (pq->size==0)
    return NULL;
  t= pq->vec[0];
  pq->size--;
  for (k= 0; k<pq->size; k++) {
    pq->vec[k]= pq->vec[k+1];
    pq->ofertas[k]= pq->ofertas[k+1];
  }
  return t;
}

void PriPut(PriQueue pq, void *t, int oferta) {
  if (pq->size==pq->maxsize)
    nFatalError("PriPut", "Desborde de la cola de prioridad\n");
  int k;
  for (k= pq->size-1; k>=0; k--) {
    if (oferta > pq->ofertas[k])
      break;
    else {
      pq->vec[k+1]= pq->vec[k];
      pq->ofertas[k+1]= pq->ofertas[k];
    }
  }
  pq->vec[k+1]= t;
  pq->ofertas[k+1]= oferta;
  pq->size++;
}

int PriSize(PriQueue pq) {
  return pq->size;
}

int PriBest(PriQueue pq) {
  return pq->size==0 ? 0 : pq->ofertas[0];
}

int EmptyPriQueue(PriQueue pq) {
  return pq->size==0;
}

int PriLength(PriQueue pq) {
  return pq->size;
}

