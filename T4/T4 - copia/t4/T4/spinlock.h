typedef struct {
  int busy;
  nMonitor m;
} SpinLock;

enum { OPEN, CLOSED };

void initSpinLock(SpinLock *psl, int ini);
void destroySpinLock(SpinLock *psl);
void spinLock(SpinLock *psl);
void spinUnlock(SpinLock *psl);
