int rojos = 0, verdes = 0;
nSem mutex; /* = nMakeSem (1); */
nSem r;     /* = nMakeSem (1); */
nSem v;     /* = nMakeSem (1); */

void entraRojo() {
    nWaitSem(r);
    if (rojos == 0)
        nWaitSem(mutex);
    rojos++;
    nSignalSem(r);
}

void saleRojo() {
    nWaitSem(r);
    rojos--;
    if (rojos == 0)
        nSignalSem(mutex);
    nSignalSem(r);
}

void entraVerde() {
    nWaitSem(v);
    if (verdes == 0)
        nWaitSem(mutex);
    verdes++;
    nSignalSem(v);
}

void saleVerde() {
    nWaitSem(v);
    verdes--;
    if (verdes == 0)
        nSignalSem(mutex);
    nSignalSem(v);
}


