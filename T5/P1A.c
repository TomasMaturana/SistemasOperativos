//////////////////////////////////////////////////////
//Función modificada//////////////////////////////////
//////////////////////////////////////////////////////
void pagefault(int page) {							//
	Process *p= current_process;					//
	int *ptab= p->pageTable;						//
//////Modificación////////////////					//
	if (bitV2(ptab[page])) {	//					//
		setBitV(&ptab[page], 1);//					//
		purgeTlb();				//					//
		}						//					//
//////////////////////////////////					//
	else{ 											//
		if (bitS(ptab[page]))						//
			pageIn(p, page, findRealPage()); 		//
		else segfault(page);            			//
	}												//
}													//
//////////////////////////////////////////////////////
	
//////////////////////////////////////////////////////
//Función modificada//////////////////////////////////
//////////////////////////////////////////////////////
void pageIn(Process *p, int page, int realPage) {	//
	int *ptab= p->pageTable;						//
	setRealPage(&ptab[page], realPage);				//
	setBitV(&ptab[page], 1);						//
//////Modificación////////////////					//
	setBitV2(&ptab[page], 1);	//					//
//////////////////////////////////					//
	loadPage(p, page);  							//
	setBitS(&ptab[page], 0);						//
	purgeTlb();         							//
	purgeL1(); 						         		//
}													//
//////////////////////////////////////////////////////

//////////////////////////////////////////////////////
//Función modificada//////////////////////////////////
//////////////////////////////////////////////////////
int pageOut(Process *q, int page) {					//
	int *qtab= q->pageTable;						//
	int realPage= getRealPage(qtab[page]);			//
	savePage(q, page);  							//
	setBitV(&qtab[page], 0);						//
//////Modificación////////////////					//
	setBitV2(&qtab[i], 0);		//					//
//////////////////////////////////					//
	setBitS(&qtab[page], 1);						//
	return realPage; 								//
}													//
//////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//Función modificada//////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void computeWS(Process *p) {										//
  int *ptab= p->pageTable;											//
  for (int i= p->firstPage; i<p->lastPage; i++) {					//
    if (bitV(ptab[i])) {											//
//////Modificación////////////////  								//
      if (bitV2(ptab[i])) {		//									//
//////////////////////////////////									//
        setBitWS(&ptab[i], 1);										//
//////Modificación////////////////  								//
        setBitV2(&ptab[i], 0);	//									//
//////////////////////////////////  								//
      }																//
      else {														//
        setBitWS(&ptab[i], 0);										//
      }																//
    }																//
  }																	//
}																	//
//////////////////////////////////////////////////////////////////////

// Variables globales
Iterator *it; 
Process *cursor_process= NULL;
int cursor_page;
//////Modificación////////
int needsSwapping= 0;	//
//////////////////////////

//////////////////////////////////////////////////////////////////////
//Función modificada//////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
int findRealPage() {												//
  needsSwapping= 0;													//
  for (;;) {														//
    int realPage= getAvailableRealPage();							//
    if (realPage>=0)												//
      return realPage;												//
    if (cursor_process==NULL) {										//
      if (!hasNext(it)) {											//
        if (needsSwapping) {										//
          doSwapping();												//
          needsSwapping= 0;											//
          continue;													//
        }															//
        resetIterator(it);											//
        needsSwapping= 1;											//
      }																//
      cursor_process= nextProcess(it);								//
      cursor_page= cursor_process->firstPage;						//
    }																//
    int *qtab= cursor_process->pageTable;							//
    while (cursor_page<=cursor_process->lastPage) {					//
//////Modificación////////////////////////////						//
	  if (bitV2(qtab[cursor_page])){		//						//
		if ((bitV(qtab[cursor_page])) &&	//						//
           !bitWS(qtab[cursor_page]))		//						//
//////////////////////////////////////////////						//
			return pageOut(cursor_process, cursor_page++);			//
	  }																//
      cursor_page++;												//
    }																//
    cursor_process= NULL;											//
  }																	//
}																	//
//////////////////////////////////////////////////////////////////////
	
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		