#include "nSystem.h"
#include "nSysimp.h"

nSem s[2]; // = nMakeSem(0) x 2
nSem m; // = nMakeSem(1) //para garantizar exclusión mutua
int cnt[2]= {0, 0};
// cnt[cat] es el n° de threads de
// categoría cat en espera.
int ocup= -1; // -1: libre
// 0 o 1: ocupado por thread de cat 0 o 1
struct Queue unos;
struct Queue ceros;

void init(){
	cnt={0,0};
	ocup= -1;
	unos= MakeQueue();
	ceros= MakeQueue();
}

void nPedir(int cat) {
	START_CRITICAL();
	if (ocup==-1){
		ocup= cat;
	}
	END_CRITICAL();
	else {
		nTask this_task = current_task;
		cnt[cat]++;
		this_task->status= WAIT_PEDIR;
		if(cat){
			PutTask(unos, this_task);
		}
		else{
			PutTask(ceros, this_task);
		}
		ResumeNextReadyTask(); // cambio de contexto
	}
}

void nDevolver() {
	if (cnt[!ocup]>0) {
		cnt[!ocup]--;
		ocup= !ocup;
		if(c->ocup){ //si ocup pasa a ser 1, se saca una task de la queue de unos y se deja READY
			nTask siguiente= GetTask(unos);
			siguiente->status= READY;
			PutTask(ready_queue , siguiente);
		}
		else{
			nTask siguiente= GetTask(ceros);
			siguiente->status= READY;
			PutTask(ready_queue , siguiente);
		}
		nTask this_task = current_task;
		PushTask(ready_queue , this_task);
		ResumeNextReadyTask(); // cambio de contexto
	}  
	else if (cnt[ocup]>0) {
		cnt[ocup]--;
		if(c->ocup){ //si ocup pasa a ser 1, se saca una task de la queue de unos y se deja READY
			nTask siguiente= GetTask(unos);
			siguiente->status= READY;
			PutTask(ready_queue , siguiente);
		}
		else{
			nTask siguiente= GetTask(ceros);
			siguiente->status= READY;
			PutTask(ready_queue , siguiente);
		}
		nTask this_task = current_task;
		PushTask(ready_queue , this_task);
		ResumeNextReadyTask(); // cambio de contexto
	}  
	else    
		ocup= -1;
}