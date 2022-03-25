int nSend(nTask task, void *msg) {
		int rc;
		START_CRITICAL(); // deshabilita interrupciones
		nTask this_task = current_task;
		if (task->status == WAIT_SEND) {
			task->status = READY; 
			/* En primer lugar en la cola */
			PushTask(ready_queue , task);
		}
		PutTask(task->send_queue, this_task);
		this_task->send.msg = msg;
		this_task->status = WAIT_REPLY;
		ResumeNextReadyTask(); // cambio de contexto
		rc = this_task->send.rc;
    END_CRITICAL();
    return rc;
}

void *nReceive(nTask *ptask, int timeout) {
    void *msg;
    nTask send_task;
    START_CRITICAL();
    nTask this_task = current_task;
    if (EmptyQueue(this_task->send_queue) && timeout != 0) {
        /* La tarea espera indefinidamente */
        this_task->status = WAIT_SEND;
        /* Se suspende indefinidamente hasta un nSend */
        ResumeNextReadyTask();
    }
    send_task = GetTask(this_task->send_queue);
    if (ptask!=NULL) 
        *ptask = send_task;
    msg = send_task == NULL ? NULL : send_task->send.msg;
    END_CRITICAL();
    return msg;
}

void nReply(nTask task, int rc) {
    START_CRITICAL();
    PushTask(ready_queue, current_task);
    task->send.rc = rc;
    task->status = READY;
    PushTask(ready_queue, task);
    ResumeNextReadyTask();
    END_CRITICAL();
}