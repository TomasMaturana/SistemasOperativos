#include "nSystem.h"
#include "nSysimp.h"
#include "fifoqueues.h"

FifoQueue hiQueue, oxQueue; 

void H2oInit() {
	hiQueue= MakeFifoQueue();
	oxQueue= MakeFifoQueue();
}


nH2o nCombineOxy(void *oxy, int timeout) {
	START_CRITICAL(); // deshabilita interrupciones
	nTask this_task = current_task;
	if(LengthFifoQueue(hiQueue)>1){
		nTask hi1 = (nTask ) GetObj(hiQueue);
		nTask hi2 = (nTask ) GetObj(hiQueue);
		
		nH2o h2o= nMalloc(sizeof(*h2o));
		h2o->hydro1= (hi1)->atom;
		h2o->hydro2= (hi2)->atom;
		h2o->oxy= oxy;
		
		this_task->mol = h2o;
		this_task->status= READY;
		PushTask(ready_queue , this_task);
		
		(hi1)->mol =h2o;
		(hi1)->status = READY;
		PushTask(ready_queue , (hi1));
		
		(hi2)->mol =h2o;
		(hi2)->status = READY;
		PushTask(ready_queue , (hi2));
	}
	else{
		this_task->mol = NULL;
		this_task->atom = oxy;
		if (timeout>0){
			this_task->status = WAIT_TIMEOUT;
			ProgramTask(timeout);
		}
		else{
			this_task->status = WAIT_PARTICULA; 
		}
		PutObj(oxQueue, this_task);
	}
	ResumeNextReadyTask(); // cambio de contexto
	if(this_task->mol==NULL){
		DeleteObj(oxQueue, this_task);
	}
	END_CRITICAL();
	return (this_task->mol);
}


nH2o nCombineHydro(void *hydro) {
	START_CRITICAL(); // deshabilita interrupciones
	nTask this_task = current_task;
	if((LengthFifoQueue(hiQueue)>0) && (LengthFifoQueue(oxQueue)>0)){
		nTask ox = (nTask ) GetObj(oxQueue);
		if(ox->status==WAIT_TIMEOUT){
			CancelTask(ox);
		}
		nTask hi = (nTask ) GetObj(hiQueue);
		
		nH2o h2o= nMalloc(sizeof(*h2o));
		h2o->hydro1= hydro;
		h2o->hydro2= (hi)->atom;
		h2o->oxy= (ox)->atom;
		
		this_task->mol= h2o;
		this_task->status= READY;
		PushTask(ready_queue , this_task);
		
		(hi)->status = READY; 
		(hi)->mol= h2o;
		PushTask(ready_queue , (hi));
		
		(ox)->mol= h2o;
		(ox)->status = READY;
		PushTask(ready_queue , (ox));		
	}
	else{
		this_task->mol = NULL;
		this_task->status = WAIT_PARTICULA; 
		this_task->atom = hydro;
		PutObj(hiQueue, this_task);
	}
	
	ResumeNextReadyTask(); // cambio de contexto
	END_CRITICAL();
	return (this_task->mol);
}












