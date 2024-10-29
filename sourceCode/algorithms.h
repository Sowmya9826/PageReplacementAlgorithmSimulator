#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "page_utils.h"

// The algorithms to evict a page from the list of pages.
void FIFO_FUNCTION(PageList *);

void LRU_FUNCTION(PageList *);

void LFU_FUNCTION(PageList *);

void MFU_FUNCTION(PageList *);

void R_FUNCTION(PageList *);


#endif