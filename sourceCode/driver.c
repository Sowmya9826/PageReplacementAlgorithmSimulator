#include "page_utils.h"
#include "algorithms.h"

int main(int argc, char *argv[])
{
    // The timestamp of simulator in milliseconds.
    int timestamp_ms = 0;
    int process_sizes[4] = {5, 11, 17, 31};
    int hits_per_iteration[NUM_ITERATIONS] = {};
    int misses_per_iteration[NUM_ITERATIONS] = {};

    void (*PageReplacementFn)(PageList *);
    
    if (argc != 2)
    {
        printf("Please provide the page replacement algorithm to use from {FIFO, LRU, LFU, MFU, RANDOM}.\n");
        return -1;
    }
    if (strcmp(argv[1], "FIFO") == 0)
    {
        PageReplacementFn = FIFO_FUNCTION;
    }
    else if (strcmp(argv[1], "LRU") == 0)
    {
        PageReplacementFn = LRU_FUNCTION;
    }
    else if (strcmp(argv[1], "LFU") == 0)
    {
        PageReplacementFn = LFU_FUNCTION;
    }
    else if (strcmp(argv[1], "MFU") == 0)
    {
        PageReplacementFn = MFU_FUNCTION;
    }
    else if (strcmp(argv[1], "RANDOM") == 0)
    {
        PageReplacementFn = R_FUNCTION;
    }
    else
    {
        printf("Please provide the page replacement algorithm to use from {FIFO, LRU, LFU, MFU, RANDOM}.\n");
        return -1;
    }

    const char *algorithm = argv[1];

    int num_processes_swapped_in = 0;

    srand(0);
    int iteration;
    for (iteration = 1; iteration <= NUM_ITERATIONS; iteration++)
    {
        printf("Starting iteration %d with algorithm %s\n", iteration, algorithm);
        int hits = 0;
        int misses = 0;
        PageList page_list;
        InitPageList(&page_list);
        Process processes[NUM_PROCESSES]; //Array to store all processes in the simulation.

        int p;
        // Initialize processes.
        for (p = 0; p < NUM_PROCESSES; p++)
        {
            processes[p].pid = p;
            processes[p].max_pages = process_sizes[rand() % 4];
            // Generate random arrival time in second [0, 60) and convert it to millisecond.
            processes[p].arrival_time_ms = (rand() % 60) * 1000;
            processes[p].duration_ms = (1 + (rand() % MAX_PROCESS_DURATION_SEC)) * 1000;
            processes[p].current_page = 0;
        }
        qsort(processes, NUM_PROCESSES, sizeof(Process), CompareProcessArrivalTime); //sort them on arrival time

        int index = 0;
        for (timestamp_ms = 0; timestamp_ms < PROGRAM_DURATION_MILLISEC; timestamp_ms += 100) //100-millisecond intervals.
        {
            // Check which processes are eligible to be scheduled.
            while (index < NUM_PROCESSES && processes[index].arrival_time_ms <= timestamp_ms)
            {
                if (CheckAtLeastFreePages(&page_list, 4)) //least 4 pages are free, it assigns a free page to the process
                {
                    Page *p = GetFreePage(&page_list);
                    p->pid = processes[index].pid;
                    p->page_number = processes[index].current_page;
                    p->first_access_timestamp_ms = timestamp_ms;
                    p->access_count = 1;
                    p->last_access_timestamp_ms = timestamp_ms;
                    num_processes_swapped_in++;
                    misses++;
                    printf("[Starting a new process] timestamp_ms: %d, process_id: %d, Enter, service_duration_ms: %d, process_size: %d\n", timestamp_ms, processes[index].pid, processes[index].duration_ms, processes[index].max_pages);
                    PrintMemoryMap(&page_list);
                    index++;
                }
                else {
                    // If less than 4 pages are available in memory, do not try to schedule any more processes.
                    break;
                }
            }
            int j;
            // Generate next pages to access for all running processes.
            //If the page is in memory, it logs a hit. If not, it logs a miss and either finds a free page or evicts a page using PageReplacementFn if memory is full.
            for (j = 0; j < index; j++)
            {
                if (processes[j].duration_ms > 0)
                {
                    processes[j].current_page = GenerateNextPageToAccess(processes[j].current_page, processes[j].max_pages);
                    // The page is already present in memory.
                    if (IsPageAlreadyInMemory(&page_list, processes[j].pid, processes[j].current_page))
                    {
                        Page *found_page = GetPageAlreadyInMemory(&page_list, processes[j].pid, processes[j].current_page);
                        found_page->access_count++;
                        found_page->last_access_timestamp_ms = timestamp_ms;
                        hits++;
                        printf("[Accessing a new page] timestamp_ms: %d, process_id: %d, page_referenced: %d, page_in_memory: true\n", timestamp_ms, processes[j].pid, processes[j].current_page);
                    }
                    else
                    {
                        printf("[Accessing a new page] timestamp_ms: %d, process_id: %d, page_referenced: %d, page_in_memory: false\n", timestamp_ms, processes[j].pid, processes[j].current_page);
                        misses++;
                        Page *free_page = GetFreePage(&page_list);
                        if (!free_page)
                        {
                            printf("Memory is full, evicting a page. Memory map before eviction:\n");
                            PrintMemoryMap(&page_list);
                            // Evict a page to make room for current page.
                            PageReplacementFn(&page_list);
                            printf("Memory map after eviction:\n");
                            PrintMemoryMap(&page_list);
                            free_page = GetFreePage(&page_list);
                        }
                        free_page->pid = processes[j].pid;
                        free_page->page_number = processes[j].current_page;
                        free_page->first_access_timestamp_ms = timestamp_ms;
                        free_page->last_access_timestamp_ms = timestamp_ms;
                        free_page->access_count = 0; // 1?
                        printf("Memory map after adding page %d for process_id %d:\n", processes[j].current_page, processes[j].pid);
                        PrintMemoryMap(&page_list);
                    }
                }
            }
            //Reduces each process’s duration. If duration_ms reaches zero, it removes the process’s pages from memory and updates the memory map
            for (j = 0; j < index; j++) {
                if (processes[j].duration_ms > 0)
                {
                    processes[j].duration_ms -= 100;
                    if (processes[j].duration_ms == 0)
                    {
                        printf("[Finishing a process] timestamp_ms: %d, process_id: %d, Exit, service_duration_ms: %d\n", timestamp_ms, processes[j].pid, processes[j].duration_ms);
                        printf("Memory map before cleanup:\n");
                        PrintMemoryMap(&page_list);
                        RemoveAllPagesOfProcess(&page_list, processes[j].pid);
                        printf("Memory map after cleanup:\n");
                        PrintMemoryMap(&page_list);
                    }
                }
            }
        }
        hits_per_iteration[iteration] = hits;
        misses_per_iteration[iteration] = misses;
    }

    printf("Average number of processes that were successfully swapped in: %f\n", (num_processes_swapped_in / (1.0*NUM_ITERATIONS)));
    int total_hits = 0;
    int total_misses = 0;
    printf("Hit rates:");
    for (iteration = 1;iteration <= NUM_ITERATIONS;iteration++) {
        total_hits += hits_per_iteration[iteration];
        total_misses += misses_per_iteration[iteration];
        printf(" %f%% ", GetHitRate(hits_per_iteration[iteration], misses_per_iteration[iteration]));
    }    
    printf("Average hit rate: %f%%\n", GetHitRate(total_hits, total_misses));
}
