#include <nSystem.h>
#include <fifoqueues.h>
#include "spinlock.h"
#include "pedir.h"

typedef struct{
	SpinLock m;
	FifoQueue cQ, uQ; // = MakeFifoQueue();
	int cnt[2];
	// cnt[cat] es el n° de threads de
	// categoría cat en espera.
	int ocup; // -1: libre
	// 0 o 1: ocupado por thread de cat 0 o 1
} Ctrl;


Ctrl *makeCtrl() {  
	Ctrl *c= malloc(sizeof(*c));
	initSpinLock(&(c->m), 0);
	c->cQ= MakeFifoQueue();
	c->uQ= MakeFifoQueue();
	c->cnt[0]= 0;
	c->cnt[1]= 0;
	c->ocup= -1;
	return c;
}

void pedirAux(int cat, Ctrl *c) {
	spinLock(&(c->m));
	if (c->ocup==-1){
		c->ocup= cat;  
		spinUnlock(&(c->m));
	}
	else {
		c->cnt[cat]++;
		SpinLock *spin= malloc(sizeof(*spin));
		initSpinLock(&(*spin), 1);
		if(cat==0){
			PutObj(c->cQ, &(*spin)); 
		}
		else{
			PutObj(c->uQ, &(*spin));
		}
		spinUnlock(&(c->m));
		spinLock(&(*spin));
		destroySpinLock(&(*spin));
		free(spin);
	}
}

void devolverAux(Ctrl *c) {
	spinLock(&(c->m));
	if (c->cnt[!c->ocup]>0) {
		c->cnt[!c->ocup]--;
		c->ocup= !c->ocup;
		if(c->ocup){ //si ocup pasa a ser 1, se saca una request de la fifo de unos
			SpinLock *pr= (SpinLock *)GetObj(c->uQ);
			spinUnlock(&(*pr));
		}
		else{
			SpinLock *pr= (SpinLock *)GetObj(c->cQ);
			spinUnlock(&(*pr));
		}
	}  
	else if (c->cnt[c->ocup]>0) {
		c->cnt[c->ocup]--;
		if(c->ocup){ //si ocup sigue siendo 1, se saca una request de la fifo de unos
			SpinLock *pr= (SpinLock *)GetObj(c->uQ);
			spinUnlock(&(*pr));
		}
		else{
			SpinLock *pr= (SpinLock *)GetObj(c->cQ);
			spinUnlock(&(*pr));
		}
	}  
	else    
		c->ocup= -1;
	spinUnlock(&(c->m));
}



void terminarAux(Ctrl *c){
	destroySpinLock(&(c->m));
	//free(c);
}

Ctrl *c;

void iniciar() {
	c=makeCtrl();
}

void terminar() {
	terminarAux(c);
}



void pedir(int cat){
    pedirAux(cat, c);
}

void devolver(){
    devolverAux(c);
}
