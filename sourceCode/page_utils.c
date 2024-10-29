#include "page_utils.h"

int CompareProcessArrivalTime(const void *left, const void *right)
{
    return ((Process *)left)->arrival_time_ms - ((Process *)right)->arrival_time_ms;
}

void InitPageList(PageList *page_list)
{
    page_list->first_page = malloc(sizeof(Page));
    page_list->num_pages = NUM_PAGES;
    Page *it = page_list->first_page;
    int i;
    for (i = 0; i < NUM_PAGES; i++)
    {
        it->pid = -1;
        it->page_number = -1;
        it->next = NULL;
        // revisit this
        if (i < NUM_PAGES-1)
        {
            it->next = malloc(sizeof(Page));
            it = it->next;
        }
    }
}

int CheckAtLeastFreePages(PageList *page_list, int threshold)
{
    Page *it = page_list->first_page;
    int found_free_pages = 0;
    while (it)
    {
        if (it->pid == -1)
        {
            found_free_pages++;
        }
        if (found_free_pages >= threshold)
            return 1;
        it = it->next;
    }
    return 0;
}

int IsPageAlreadyInMemory(PageList *page_list, int pid, int page_number)
{
    Page *it = page_list->first_page;
    while (it)
    {
        if (it->pid == pid && it->page_number == page_number)
            return 1;
        it = it->next;
    }
    return 0;
}

Page *GetPageAlreadyInMemory(PageList *page_list, int pid, int page_number)
{
    Page *it = page_list->first_page;
    while (it)
    {
        if (it->pid == pid && it->page_number == page_number)
            return it;
        it = it->next;
    }
    return NULL;
}

Page *GetFreePage(PageList *page_list)
{
    Page *it = page_list->first_page;
    while (it)
    {
        if (it->pid == -1)
            return it;
        it = it->next;
    }
    return NULL;
}

void RemoveAllPagesOfProcess(PageList *page_list, int pid)
{
    Page *it = page_list->first_page;
    while (it)
    {
        if (it->pid == pid)
        {
            it->pid = -1;
            it->page_number = -1;
        }
        it = it->next;
    }
}

void PrintMemoryMap(PageList *page_list)
{
    Page *it = page_list->first_page;
    int cnt = 0;
    while (it)
    {
        if (it->pid == -1) {
            printf("|%03d: ***[**][f:*****, l:*****, c:***]|", cnt);
        } else {
            printf("|%03d: %03d[%02d][f:%05d, l:%05d, c:%03d]|", cnt, it->pid, it->page_number, it->first_access_timestamp_ms, it->last_access_timestamp_ms, it->access_count);
        }
        cnt++;
        it = it->next;
    }
    printf("\n");
}

// Assume i is the current page number
// * To compute the next i, first generate a random number “r” from 0 through 10.
// * If 0 ≤ r < 7, then generate a random ∆i to be -1, 0, or +1.
// * For 7 ≤ r ≤ 10, randomly generate the new page reference “j,” 2 ≤ |∆i| ≤ 9
// [ 0 ≤ j ≤ i-2 or i+2 ≤ j ≤ 10]
int GenerateNextPageToAccess(int curr_page_no, int max_page_size)
{
    int r = rand() % 10;
    int next_page_no = curr_page_no;
    if (r < 7)
    {
        int delta = (rand() % 3) - 1; // {0,1,2} - 1 => {-1, 0, 1}
        next_page_no = curr_page_no + delta;
    }
    else
    {
        int sign = (rand() % 2) == 0 ? -1 : 1;
        int delta = sign * ((rand() % 8) + 2); // sign * ([0, 7] + 2) => sign * ([2, 9])
        next_page_no = curr_page_no + delta;
    }
    // Wrap around in case the next page number is negative or greater than max_page_size.
    return (next_page_no + max_page_size) % max_page_size;
}

float GetHitRate(int hits, int misses) {
    return (100.0*hits) / (1.0*(hits + misses));
}