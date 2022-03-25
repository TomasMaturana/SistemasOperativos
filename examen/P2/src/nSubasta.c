#include <nSystem.h>
#include "nSysimp.h"

// Encabezados para la cola de prioridades

typedef struct priqueue *PriQueue;

PriQueue MakePriQueue(int size);
void *PriGet(PriQueue pq);
void PriPut(PriQueue pq, void* t, int pri);
int PriSize(PriQueue pq);
int PriBest(PriQueue pq);
int EmptyPriQueue(PriQueue pq);
int PriLength(PriQueue pq);

// Defina aca el tipo struct subasta
// El typedef struct subasta *nSubasta esta en nSystem.h

struct subasta {  
	PriQueue c; // cola de prioridades  
	int n;      // número de unidades
	//SpinLock m;
};




// Programe aca las funciones nuevaSubasta, ofrecer y adjudicar, mas
// otras funciones que necesite.

#define Resume ResumeNextReadyTask

nSubasta nNuevaSubasta(int n) {  
	nSubasta s= malloc(sizeof(*s));  
	s->n= n;  
	s->c= MakePriQueue(100);  
	//initSpinLock(&(s->m), 0);
	return s;
}

int nOfrecer(nSubasta s, int oferta) { 
	START_CRITICAL();
	nTask this_task = current_task;
	this_task->oferta=oferta;
	this_task->resol=-1; 
	//initSpinLock(&(r.w), 1);
	//spinLock(&(s->m));
	//this_task->status= WAIT_SUBASTA;
	this_task->status= WAAIIT;
	PriPut(s->c, this_task, oferta);  
	if (PriSize(s->c)>s->n) {    
		nTask pr= (nTask) PriGet(s->c);    
		pr->resol= 0; // se rechaza
		pr->status= READY;
		PushTask(ready_queue , pr);		
		//spinUnlock(&(pr->w));
	}  
	//spinUnlock(&(s->m));
	if (this_task->resol == -1) ResumeNextReadyTask();
	END_CRITICAL();
	return this_task->resol;
}

int nAdjudicar(nSubasta s, int *punid) { 
	START_CRITICAL();
	nTask this_task = current_task;
	int recaud= 0;  
	*punid= 0;  
	//spinLock(&(s->m));
	while (!EmptyPriQueue(s->c)) {    
		nTask pr= PriGet(s->c);    
		pr->resol= 1; // se adjudica    
		(*punid)++;    
		recaud += pr->oferta;  
		pr->status= READY;
		PushTask(ready_queue , pr);	
		//spinUnlock(&(pr->w));
	}  
	s->n -= *punid;
	this_task->status= READY;
	PushTask(ready_queue , this_task);	
	ResumeNextReadyTask();
	//spinUnlock(&(s->m));
	//destroySpinLock(&(s->m));
	END_CRITICAL();
	return recaud;
}




// Implementacion de la cola de prioridades: dada

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
