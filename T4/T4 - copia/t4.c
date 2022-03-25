#include <nSystem.h>
#include <fifoqueues.h>
#include "spinlock.h"
#include "pedir.h"


FifoQueue unos, ceros;
SpinLock s;
int n;
int cnt[2]= {0, 0};

typedef struct {
    int ready;
} Request;

void iniciar() {
  unos = MakeFifoQueue();
  ceros = MakeFifoQueue();
  initSpinLock(&s, OPEN);
  n = -1;
}

void terminar() {
  destroySpinLock(&s);
  DestroyFifoQueue(unos);
  DestroyFifoQueue(ceros);
}

void pedir(int cat) {
  nPrintf("Se pide %d, con n = %d, cnt de 0 es: %d, y cnt de 1 es: %d \n", cat, n, cnt[0], cnt[1]);
  //nPrintf("Se pide, n es %d: \n", n);
  spinLock(&s);
  if (n > -1) {
    //nPrintf("entro aquí \n");
    cnt[cat]++;
    //nPrintf("cat value: %d \n", cat);
    if (cat == 1) {
      //nPrintf("hola xd, cat: %d \n", cat);
      SpinLock task_uno; 
      //nPrintf("creo el spinlock \n");
      initSpinLock(&task_uno, CLOSED);
      //nPrintf("incio el spinlock cerrado \n"); 
      PutObj(unos, &task_uno);
      spinUnlock(&s);
      //nPrintf("se guarda \n");
      spinLock(&task_uno);     
      //nPrintf("lo lockeo? \n");
    }
    else if (cat == 0) {
      //nPrintf("hola xd, cat debe ser 0: %d \n", cat);
      SpinLock task_cero; 
      initSpinLock(&task_cero, CLOSED);
      PutObj(ceros, &task_cero);
      //nPrintf("se guarda \n");      
      spinUnlock(&s);
      spinLock(&task_cero); 
    }
  }
  else {
    //nPrintf("primer thread \n");
    n = cat;
    spinUnlock(&s);
    //nPrintf("Se hace unlock \n");
  }
  //nPrintf("termine de pedir \n");
}

void devolver() {
  nPrintf("Se devuelve n = %d, cnt de 0 es: %d, y cnt de 1 es: %d \n", n, cnt[0], cnt[1]);
  spinLock(&s);
  //nPrintf("Estoy devolviendo y n es: %d \n", n);
  if(n == 1) {
    //nPrintf("Entré al if para 1 \n");
    if(cnt[0] > 0|| !EmptyFifoQueue(ceros)) {
      n = 0;
      //nPrintf("Entré al ACAAA if \n");
      //nPrintf("hago unlock del mutex\n");
      //nPrintf("n actual: %d \n", n);
      SpinLock * cero = GetObj(ceros);
      //nPrintf("inicio el spinlock \n");
      spinUnlock(cero);
      //nPrintf("hago unlock \n");
      destroySpinLock(cero);
      cnt[0]--;
      //nPrintf("se destruye \n");
      spinUnlock(&s);

    }
    else if (EmptyFifoQueue(ceros) && !EmptyFifoQueue(unos)){
      SpinLock * uno = GetObj(unos);
      //nPrintf("inicio el spinlock \n");
      spinUnlock(uno);
      //nPrintf("hago unlock \n");
      destroySpinLock(uno);
      cnt[1]--;
      spinUnlock(&s);

    } 
    else {
      n = -1;
      spinUnlock(&s);
    }
    //spinUnlock(&s);
  }
  else  {
    //nPrintf("Entré al primer if para 0 \n");
    if(cnt[1] > 0 || !EmptyFifoQueue(unos)) {
      //nPrintf("Entré al AQUIII if \n");
      //nPrintf("hago unlock del mutex \n");
      //nPrintf("cambié la variable n a 1 \n");
      SpinLock * uno = GetObj(unos);
      //nPrintf("inicio el spinlock \n");
      spinUnlock(uno);
      //nPrintf("hago unlock \n");
      destroySpinLock(uno);
      cnt[1]--;
      //nPrintf("se destruye \n");
      n = 1;
      spinUnlock(&s);
    }
    else if (EmptyFifoQueue(unos) && !EmptyFifoQueue(ceros)) {
      SpinLock * cero = GetObj(ceros);
      //nPrintf("inicio el spinlock \n");
      spinUnlock(cero);
      //nPrintf("hago unlock \n");
      destroySpinLock(cero);
      cnt[0]--;
      spinUnlock(&s);
      //nPrintf("se destruye \n");
    }
    else {
      n = -1;
      spinUnlock(&s);

    }
    
    //spinUnlock(&s);

  }
  //spinUnlock(&s);

}
