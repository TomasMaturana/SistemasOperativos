#include <nSystem.h>
#include "fifoqueues.h"
#include "pub.h"
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

typedef struct{
	nMonitor m;
	FifoQueue dQ, vQ; // = MakeFifoQueue();
	int damas, varones;
} Ctrl;

typedef struct{
	int kind; //damas = 1, varones = 0
	nCondition w;
	int ready;
} Request;

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

Ctrl *makeCtrl() {  
	//Ctrl *c= (Ctrl)nMalloc(sizeof(*c));
	Ctrl *c= nMalloc(sizeof(*c));
	c->m= nMakeMonitor();
	c->dQ= MakeFifoQueue();
	c->vQ= MakeFifoQueue();
	c->damas= 0;
	c->varones= 0;
	return c;
}

void await(Ctrl *c , int kind){
	Request r;
	r.kind= kind;
	r.w= nMakeCondition(c->m);
	r.ready = 0;
	if(kind==VARON){
		PutObj(c->vQ, &r); /* al final de q */  
	}
	else{
		PutObj(c->dQ, &r); /* al final de q */  
	}
	nWaitCondition(r.w);
	nDestroyCondition(r.w);
}

void wakeupDamas(Ctrl *c) {  
	Request *pr= (Request*)GetObj(c->dQ);   
	if (pr==NULL){
		return; //desde aquí damas comienzan a esperar en dQ, hasta que vuelvan a entrar damas
	}
	if (c->varones==0){
		pr->ready= 1;
		nSignalCondition(pr->w);
	}
	else /* se devuelve al comienzo de q */    
		PushObj(c->dQ, pr);
}

void wakeupVarones(Ctrl *c) {  
	Request *pr= (Request*)GetObj(c->vQ);   
	if (pr==NULL){
		return; //desde aquí varones comienzan a esperar en vQ, hasta que vuelvan a entrar varones
	}
	if (c->damas==0){
		pr->ready= 1;
		nSignalCondition(pr->w);
	}
	else /* se devuelve al comienzo de q */    
		PushObj(c->vQ, pr);
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void entrarDama(Ctrl *c) {  
	nEnter(c->m);
	if (c->varones>0 || !EmptyFifoQueue(c->vQ) || (c->varones==0 && !EmptyFifoQueue(c->dQ))){   // si hay varones en el baño o en la cola, la dama se pone en la cola de damas
		await(c, DAMA);
	}
	c->damas++;
	wakeupDamas(c); 
	nExit(c->m);
}

void salirDama(Ctrl *c) {  
	nEnter(c->m);
	c->damas--;
	if (c->damas==0){   
		wakeupVarones(c);
	}
	nExit(c->m);
}

void entrarVaron(Ctrl *c) {  
	nEnter(c->m);
	if (c->damas>0 || !EmptyFifoQueue(c->dQ) || (c->damas==0 && !EmptyFifoQueue(c->vQ))){     // si hay damas en el baño o en la cola, el varón se pone en la cola de varones
		await(c, VARON);
	}
	c->varones++;
	wakeupVarones(c);
	nExit(c->m);
}

void salirVaron(Ctrl *c) {  
	nEnter(c->m);
	c->varones--;
	wakeupDamas(c);
	nExit(c->m);
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
    
Ctrl c;

void ini_pub(void){
	c=*makeCtrl();	
}


void entrar(int sexo){
	if (sexo){ //entran damas
		entrarDama(&c);
	}
	else{      //entran varones
		entrarVaron(&c);
	}
}


void salir(int sexo){
	if(sexo){  //salen damas
		salirDama(&c);
	}
	else{      //salen varones
		salirVaron(&c);
	}
}








