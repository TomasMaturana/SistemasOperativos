
/*************************************************************
 * nQueue.c
 *************************************************************/

#define TYPE_QUEUE  1
#define TYPE_SQUEUE 2
/*
 * Manejo de colas FIFO
 */

typedef struct Queue {
  int   type;
  nTask first;
  nTask *last;
}
  *Queue; // Atencion, una cola es un puntero

/* Las siguientes funciones deben ser siempre invocadas con las
 * interrupciones deshabilitadas
 */

Queue MakeQueue();                      // El constructor
void PutTask(Queue queue, nTask task);  // Agrega una tarea al final
void PushTask(Queue queue, nTask task); // Agrega una tarea al principio
nTask GetTask(Queue queue);             // Extrae la primera tarea
int EmptyQueue(Queue queue);            // Verdadero si la cola esta vacia
int QueueLength(Queue queue);           // Entrega el largo de la cola
int QueryTask(Queue queue, nTask task); // Verdadero si task esta en la cola
void DeleteTaskQueue(Queue queue, nTask task ); // Borra task de la cola
void DestroyQueue(Queue queue);         // El destructor

#define DeleteTaskInQueue DeleteTaskQueue

/*
 * Manejo de colas ordenadas por tiempo
 */

typedef struct Squeue {
  int type;
  nTask first;
}
  *Squeue;

Squeue MakeSqueue();                   // El constructor
void PutTaskSqueue(Squeue squeue, nTask task, int wake_time); // Agrega task
nTask GetTaskSqueue(Squeue squeue);    // Extrae la tarea con wake_time menor
int GetNextTimeSqueue(Squeue squeue);  // Entrega el wake_time menor
int EmptySqueue(Squeue squeue);        // Verdadero si la cola esta vacia
void DeleteTaskSqueue(Squeue squeue, nTask task); // Borra task de la cola
void DestroySqueue(Squeue squeue);     // El destructor

#define DeleteTaskInSqueue DeleteTaskSqueue
