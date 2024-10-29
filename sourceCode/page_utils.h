#ifndef PAGE_UTILS_H
#define PAGE_UTILS_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define NUM_ITERATIONS 5
// Change for mini demo.
// #define NUM_PROCESSES 20
#define NUM_PROCESSES 150
#define PROGRAM_DURATION_SEC 60
#define PROGRAM_DURATION_MILLISEC 60000
#define MAX_PROCESS_DURATION_SEC 5
#define NUM_PAGES 100
// Change for mini demo.
// #define NUM_PAGES 5

typedef struct Process
{
    // Process Id.
    int pid;
    // The timestamp from whic the process can be scheduled.
    int arrival_time_ms;
    // Each page is 1 MB in size. The size of process is max_pages * 1 MB.
    int max_pages;
    // The total duration of the process in milliseconds.
    int duration_ms;
    // The page which the process is currently accessing.
    // The value ranges from [0, max_pages).
    int current_page;
} Process;

// Compare processes with respect to arrival time.
// This comparator is used to sort the processes in increasing order of arrival time.
int CompareProcessArrivalTime(const void *, const void *);

typedef struct Page
{
    // The process id to which this page belongs to.
    int pid;
    // The page number of the process this page represents.
    int page_number;
    // Time when the page was added to the page list.
    int first_access_timestamp_ms;
    // Time the page was last accessed.
    int last_access_timestamp_ms;
    // Number of times the page was accessed.
    int access_count;
    // Pointer to next page in the list.
    struct Page *next;
} Page;

typedef struct PageList
{
    // The first page in the list.
    Page *first_page;
    // Total number of pages in the list.
    int num_pages;
} PageList;

// Initialize the page list.
void InitPageList(PageList *);

// Checks if number of free pages are greater than or equal to the input.
int CheckAtLeastFreePages(PageList *, int);

// Checks if the input page from the input process id already exists in the page list.
int IsPageAlreadyInMemory(PageList *, int, int);

// Returns the page for the input process and page number which already exists in the input page list.
// Returns NULL if the page does not exist in page list.
Page *GetPageAlreadyInMemory(PageList *, int, int);

// If there is a free page in the input page list, return it. Otherwise return NULL.
Page *GetFreePage(PageList *);

// Removes all the pages of the input process id from the input page list.
void RemoveAllPagesOfProcess(PageList *, int);

// Prints the memory map to stdout.
void PrintMemoryMap(PageList *);

// Generate the next page number which the input process with access next.
int GenerateNextPageToAccess(int, int);

// Returns the hit rate.
float GetHitRate(int hits, int misses);

#endif
