/* Declaracion de estructuras de datos: 0.3 puntos */
typedef enum {   /* posibles mensajes que se enviaran */
    SEMSIGNAL,
    SEMWAIT
} SemOp;

nTask semServer;    /* = nEmitTask(semProc); en el nMain */

/* Envio de mensajes al servidor: 0.5 puntos */
void signal() {
    SemOp op = SEMSIGNAL;
    nSend(semServer, &op);
}

void wait() {
    SemOp op = SEMWAIT;
    nSend(semServer, &op);
}

/* Servidor utilizado para el semaforo */
int semProc() {
    int c = 0;  /* contador de tickets */
    FifoQueue q = MakeFifoQueue(); /* cola para el semaforo */
    for (;;) {  /* Ciclo y recepcion de mensajes: 0.5 */
        nTask t;
        SemOp *pop = (SemOp*) nReceive(&t, -1);
        if (*pop == SEMSIGNAL) {    /* contabilizar los signal: 0.5 */
            c++;
            nReply(t, 0);
        }
        else {  /* wait: encolar: 0.5 
                 aunque hayan tickets disponibles se encola,
                 luego se sacaran de la cola.
                 PROPUESTO: hacerlo sin encolar a todos, o sea, 
                 si ya hay tickets no encolar */
            PutObj(q, t);
        }   
        /* Desencolar a los que pueden continuar : 0.7 puntos */
        while (c>0 && !EmptyFifoQueue(q)) {
            c--;
            nReply((nTask) GetObj(q), 0);
        }
    }
}
