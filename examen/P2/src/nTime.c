#include <sys/time.h> /* Para gettimeofday */

#include "nSystem.h"
#include "nSysimp.h"

/* Procedimientos locales del modulo: */

static void RtimerHandler();
static void AwakeTasks();

/* Variables del modulo */

static Squeue wait_squeue;
static int init_time=0;

/*************************************************************
 * Prologo y epilogo:
 *************************************************************/

void TimeInit(void) {
  wait_squeue= MakeSqueue();
  init_time= nGetTime();
}

void TimeEnd(void) {
  if (!EmptySqueue(wait_squeue))
    nFprintf(2, "\nQuedan tareas con timeout programados\n");
}

/*************************************************************
 * Manejo de timeouts
 *************************************************************/

int nGetTime(void) {
    struct timeval Timeval;

    gettimeofday(&Timeval, NULL);
    return Timeval.tv_sec*1000+Timeval.tv_usec/1000-init_time;
}   

void ProgramTask(int timeout) {
  VerifyCritical("ProgramTask");
  if (timeout>0) {
    int curr_time= nGetTime();
    int wake_time= curr_time+timeout;

    if (EmptySqueue(wait_squeue) || wake_time-GetNextTimeSqueue(wait_squeue)<0)
      SetAlarm(REALTIMER, wake_time-curr_time, RtimerHandler);

    PutTaskSqueue(wait_squeue, current_task, wake_time);
  }
  else {
    current_task->status= READY;
    PushTask(ready_queue, current_task);
  }
}

void CancelTask(nTask task) {
  VerifyCritical("CancelTask");
  DeleteTaskSqueue(wait_squeue, task);
  AwakeTasks();
}

static void AwakeTasks() {
  int curr_time= nGetTime();

  /* Despertamos todas las tareas con wake_time<=curr_time */

  while (! EmptySqueue(wait_squeue) &&
         GetNextTimeSqueue(wait_squeue)<=curr_time) {
    nTask task= GetTaskSqueue(wait_squeue);
    /* Ahora la tarea que dormia vuelve a estar READY */   
    task->status= READY;
    PushTask(ready_queue, task);
  }

  /* Preparamos la proxima interrupcion */
  SetAlarm(REALTIMER,
           EmptySqueue(wait_squeue) ?
             0 : GetNextTimeSqueue(wait_squeue)-curr_time,
           RtimerHandler);
}

static void RtimerHandler(void) {
  PreemptTask();

    AwakeTasks();

  ResumePreemptive();
}

/*************************************************************
 * nSleep
 *************************************************************/

void nSleep(int delay) {
  START_CRITICAL();

  current_task->status= WAIT_SLEEP;
  ProgramTask(delay);
  ResumeNextReadyTask();

  END_CRITICAL();
}

