int rojos = 0, verdes = 0;
nSem mutex; /* = nMakeSem (1); */
nSem r;     /* = nMakeSem (1); */
nSem v;     /* = nMakeSem (1); */
nSem capacidad; /* nMakeSem (4); */

void entraRojo() {
    nWaitSem(r);
    if (rojos == 0)
        nWaitSem(mutex);
    rojos++;
    nSignalSem(r);
    nWaitSem(capacidad);
}

void saleRojo() {
    nWaitSem(r);
    rojos--;
    if (rojos == 0)
        nSignalSem(mutex);
    nSignalSem(r);
    nSignalSem(capacidad);
}

void entraVerde () {
    nWaitSem(v);
    if (verdes == 0)
        nWaitSem(mutex);
    verdes++;
    nSignalSem(v);
    nWaitSem(capacidad);
}

void saleVerde () {
    nWaitSem(v);
    verdes--;
    if (verdes == 0)
        nSignalSem(mutex);
    nSignalSem(v);
    nSignalSem(capacidad);
}
