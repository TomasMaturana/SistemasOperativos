#include <nSystem.h>

#include "equipo.h"

char **equipo; 
int k;     
nSem m;
nSem *tickets; 

void init_equipo(void) {
	equipo = nMalloc(5*sizeof(char*)); 
	k= 0;     
	m = nMakeSem(1);
	tickets= nMalloc(4*sizeof(nSem));
}

char **hay_equipo(char *nom) {
	nWaitSem(m);  
	equipo[k++]= nom;
	char **miequipo= equipo;  
	if (k!=5){ 
		nSem miticket= nMakeSem(0);
		tickets[k-1]= miticket;
		nSignalSem(m); 
		nWaitSem(miticket);  
		nDestroySem(miticket);
	}
	else { 
		for (int i= 0; i<4; i++)      
			nSignalSem(tickets[i]);   
		tickets= nMalloc(4*sizeof(nSem));
		equipo= nMalloc(5*sizeof(char*));    
		k= 0;
		nSignalSem(m); 
	}
	return miequipo;
}

