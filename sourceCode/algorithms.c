#include "page_utils.h"

void PrintEvictedPageDebugInfo(Page *candidate)
//debug information helps track which pages are being evicted and the conditions of their eviction.
{
    printf("[Page Eviction] process_id: %03d, page_number: %02d, first_access_timestamp_ms:%d, last_access_timestamp_ms: %d, access_count: %d\n", candidate->pid, candidate->page_number, candidate->first_access_timestamp_ms, candidate->last_access_timestamp_ms, candidate->access_count);
}

void FIFO_FUNCTION(PageList *page_list)
{
    Page *it = page_list->first_page;
    Page *candidate = page_list->first_page;

    while (it)
    {
        if (it->first_access_timestamp_ms < candidate->first_access_timestamp_ms)
        {
            candidate = it;
        }
        it = it->next;
    }
    PrintEvictedPageDebugInfo(candidate);
    candidate->pid = -1;
    candidate->page_number = -1;
}

void LRU_FUNCTION(PageList *page_list)
{
    Page *it = page_list->first_page;
    Page *candidate = page_list->first_page;
    while (it)
    {
        if ((it->last_access_timestamp_ms < candidate->last_access_timestamp_ms) || (it->last_access_timestamp_ms == candidate->last_access_timestamp_ms && it->first_access_timestamp_ms < candidate->first_access_timestamp_ms))
        {
            candidate = it;
        }
        it = it->next;
    }
    PrintEvictedPageDebugInfo(candidate);
    candidate->pid = -1;
    candidate->page_number = -1;
}

void LFU_FUNCTION(PageList *page_list)
{
    Page *it = page_list->first_page;
    Page *candidate = page_list->first_page;
    while (it)
    {
        if ((it->access_count < candidate->access_count) || (it->access_count == candidate->access_count && it->last_access_timestamp_ms < candidate->last_access_timestamp_ms))
        {
            candidate = it;
        }
        it = it->next;
    }
    PrintEvictedPageDebugInfo(candidate);
    candidate->pid = -1;
    candidate->page_number = -1;
}

void MFU_FUNCTION(PageList *page_list)
{
    Page *it = page_list->first_page;
    Page *candidate = page_list->first_page;
    while (it)
    {
        if (it->access_count > candidate->access_count)
        {
            candidate = it;
        }
        it = it->next;
    }
    PrintEvictedPageDebugInfo(candidate);
    candidate->pid = -1;
    candidate->page_number = -1;
}

void R_FUNCTION(PageList *page_list)
{
    Page *it = page_list->first_page;
    Page *candidate = page_list->first_page;
    int evict_idx = rand() % NUM_PAGES;
    int idx = 0;
    while (it)
    {
        if (idx == evict_idx)
        {
            candidate = it;
            break;
        }
        idx++;
        it = it->next;
    }
    PrintEvictedPageDebugInfo(candidate);
    candidate->pid = -1;
    candidate->page_number = -1;
}
