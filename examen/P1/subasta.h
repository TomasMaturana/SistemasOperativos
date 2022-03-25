typedef struct subasta *Subasta;

Subasta nuevaSubasta(int n);
int ofrecer(Subasta s, int oferta);
int adjudicar(Subasta s, int *punid);

typedef struct priqueue *PriQueue;

PriQueue MakePriQueue(int size);
void *PriGet(PriQueue pq);
void PriPut(PriQueue pq, void* t, int pri);
int PriSize(PriQueue pq);
int PriBest(PriQueue pq);
int EmptyPriQueue(PriQueue pq);
int PriLength(PriQueue pq);

typedef struct {
  int busy;
  nMonitor m;
} SpinLock;

enum { OPEN, CLOSED };

void initSpinLock(SpinLock *psl, int ini);
void destroySpinLock(SpinLock *psl);
void spinLock(SpinLock *psl);
void spinUnlock(SpinLock *psl);
