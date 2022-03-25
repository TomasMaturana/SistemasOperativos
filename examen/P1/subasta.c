#include <nSystem.h>

#include "subasta.h"


struct subasta {  
	PriQueue c; // cola de prioridades  
	int n;      // número de unidades
	SpinLock m;
};

typedef struct {  
	int oferta; // dinero ofrecido  
	int resol; // estado de la oferta: -1 pendiente,             
				//  rechazada, 1 aprobada.
	SpinLock w;
} Req;

Subasta nuevaSubasta(int n) {  
	Subasta s= malloc(sizeof(*s));  
	s->n= n;  
	s->c= MakePriQueue(100);  
	initSpinLock(&(s->m), 0);
	return s;
}

int ofrecer(Subasta s, int oferta) {  
	Req r= {oferta, -1}; 
	initSpinLock(&(r.w), 1);
	spinLock(&(s->m));
	PriPut(s->c, &r, oferta);  
	if (PriSize(s->c)>s->n) {    
		Req *pr= PriGet(s->c);    
		pr->resol= 0; // se rechaza  
		spinUnlock(&(pr->w));
	}  
	spinUnlock(&(s->m));
	if (r.resol == -1) spinLock(&(r.w));
	return r.resol;
}

int adjudicar(Subasta s, int *punid) {  
	int recaud= 0;  
	*punid= 0;  
	spinLock(&(s->m));
	while (!EmptyPriQueue(s->c)) {    
		Req *pr= PriGet(s->c);    
		pr->resol= 1; // se adjudica    
		(*punid)++;    
		recaud += pr->oferta;  
		spinUnlock(&(pr->w));
	}  
	s->n -= *punid;
	spinUnlock(&(s->m));
	destroySpinLock(&(s->m));
	return recaud;
}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	