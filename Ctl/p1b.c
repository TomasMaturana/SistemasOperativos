#include <nSystem.h>
#include <fifoqueues.h>


typedef struct{
	nMonitor m;
	FifoQueue cQ, uQ; // = MakeFifoQueue();
	int cnt[2];
	// cnt[cat] es el n° de threads de
	// categoría cat en espera.
	int ocup; // -1: libre
	// 0 o 1: ocupado por thread de cat 0 o 1
} Ctrl;

typedef struct{
	int kind; //ceros = 0, unos = 1
	nCondition w;
} Request;



Ctrl *makeCtrl() {  
	Ctrl *c= nMalloc(sizeof(*c));
	c->m= nMakeMonitor();
	c->cQ= MakeFifoQueue();
	c->uQ= MakeFifoQueue();
	c->cnt[0]= 0;
	c->cnt[1]= 0;
	c->ocup= -1;
	return c;
}

void await(Ctrl *c , int kind){
	Request r;
	r.kind= kind;
	r.w= nMakeCondition(c->m);
	if(kind==0){
		PutObj(c->cQ, &r); /* al final de q */  
	}
	else{
		PutObj(c->uQ, &r); /* al final de q */  
	}
	nWaitCondition(r.w);
	nDestroyCondition(r.w);
}



void pedirAux(int cat, Ctrl *c) {
	nEnter(c->m);
	if (c->ocup==-1){
		c->ocup= cat;  
		nExit(c->m);
	}
	else {
		nExit(c->m);
		c->cnt[cat]++;
		await(c, cat);
	}
}


void devolverAux(Ctrl *c) {
	if (c->cnt[!c->ocup]>0) {
		c->cnt[!c->ocup]--;
		c->ocup= !c->ocup;
		if(c->ocup){ //si ocup pasa a ser 1, se saca una request de la fifo de unos
			Request *pr= (Request*)GetObj(c->uQ);
			nSignalCondition(pr->w);
		}
		else{
			Request *pr= (Request*)GetObj(c->cQ);
			nSignalCondition(pr->w);
		}
	}  
	else if (c->cnt[c->ocup]>0) {
		c->cnt[c->ocup]--;
		if(c->ocup){ //si ocup sigue siendo 1, se saca una request de la fifo de unos
			Request *pr= (Request*)GetObj(c->uQ);
			nSignalCondition(pr->w);
		}
		else{
			Request *pr= (Request*)GetObj(c->cQ);
			nSignalCondition(pr->w);
		}
	}  
	else    
		c->ocup= -1;
}

Ctrl c;

//////////////////////////////////////////////////
//Con init() se inicializan las variables globales//
//////////////////////////////////////////////////

void init(void){
	c=*makeCtrl();	
}
//////////////////////////////////////////////////

void pedir(int cat){
    pedirAux(cat, &c);
}

void devolver(){
    devolverAux(&c);
}
