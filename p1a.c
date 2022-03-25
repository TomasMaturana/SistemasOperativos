#include <nSystem.h>

nSem s[2]; // = nMakeSem(0) x 2
nSem m; // = nMakeSem(1) //para garantizar exclusión mutua
int cnt[2];
// cnt[cat] es el n° de threads de
// categoría cat en espera.
int ocup; // -1: libre
// 0 o 1: ocupado por thread de cat 0 o 1

void init() {
	s[0]= nMakeSem(0);
	s[1]= nMakeSem(0);
	m= nMakeSem(0);
	cnt= {0, 0};
	ocup= -1;
}

void pedir(int cat) {
	nWaitSem(m);
	if (ocup==-1)    
		ocup= cat;  
	nSignalSem(m);
	else {    
		cnt[cat]++;
		nWaitSem(s[cat]);
	}
}

void devolver() {
	if (cnt[!ocup]>0) {
		cnt[!ocup]--;
		ocup= !ocup;
		nSignalSem(s[ocup]);
	}  
	else if (cnt[ocup]>0) {
		cnt[ocup]--;
		nSignalSem(s[ocup]);
		// ¡ocup se mantiene!  
	}  
	else    
		ocup= -1;
}