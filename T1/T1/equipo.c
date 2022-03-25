#include <nSystem.h>

#include "equipo.h"

char **equipo; 
int k;     
nSem m;
nSem *tickets[4]; 

void init_equipo(void) {
	equipo = nMalloc(5*sizeof(char*)); 
	k= 0;     
	m = nMakeSem(1);
}

char **hay_equipo(char *nom) {
	nWaitSem(m);  
	int kj=k;
	equipo[k++]= nom;
	char **miequipo= equipo;  
	if (k!=5){ 
		nSignalSem(m); 
		tickets[kj]=nMakeSem(0);
		nWaitSem(tickets[kj]);  
		nDestroySem(tickets[kj]);
	}
	else { 
		for (int i= 0; i<4; i++)      
			nSignalSem(tickets[i]);     
		equipo= nMalloc(5*sizeof(char*));    
		k= 0;
		nSignalSem(m); 
	}
	return miequipo;
}

