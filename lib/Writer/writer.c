#include "hardware/flash.h"
#include "hardware/sync.h"
#include <stdlib.h>
#include <string.h>
#include "structs.h"

#define TOTAL_FLASH_SIZE 2097152

// This variable is automatically assigned the memory location
// of the end of the program on the flash storage. I.E. it
// signifies the first place that's safe to write.
extern char* __flash_binary_end;

// This stores the current page number that the program should
// write to, starting from 0 at the end of the program itself.
unsigned int currentPage;

// This stores the first safe write location, as an offset into
// the flash memory rather than an absolute address.
int writeStartLoc;

/**
 * Routine to write a page to the flash memory
 */
void writePage(struct Event eventToWrite) {
    int writeLoc = writeStartLoc + (currentPage * FLASH_PAGE_SIZE);
    void* writeBuffer = malloc(FLASH_PAGE_SIZE);
    memcpy(writeBuffer, &eventToWrite, sizeof(struct Event));
    flash_range_program(writeLoc, writeBuffer, FLASH_PAGE_SIZE);
    currentPage++;
    return;
}

/**
 * Routine to read a page from memory
 */
struct Event *readPage(int pageNumber) {
    int readLoc = XIP_BASE + writeStartLoc + (pageNumber * FLASH_PAGE_SIZE);
    struct Event *readEvent = (struct Event *)readLoc;
    return readEvent;
}

/**
 * Routine to find the number of pages that have been written/
 * the number of the next free page.
 */
int findCurrentPage(int start, int end) {
    int midpoint = (start + end)/2;
    struct Event *thisEvent = readPage(midpoint);
    if ((start == end) && (*thisEvent).eventType == None) {
        return midpoint;
    } else if ((*thisEvent).eventType != None) {
        return findCurrentPage(midpoint + 1, end);
    } else {
        return findCurrentPage(start, midpoint);
    }
}

void writer_init(struct Status *currentStatus) {
    // Initialize variables
    uintptr_t flash_end = (uintptr_t) &__flash_binary_end;
    writeStartLoc = (flash_end + (256 - (flash_end % 256)) - XIP_BASE);
    int totalPages = (TOTAL_FLASH_SIZE - writeStartLoc)/256;
    currentPage = findCurrentPage(0, totalPages - 1);
    for (int i = 0; i < currentPage; i++) {
        struct Event *thisEvent = readPage(i);
        if ((*thisEvent).eventType == Enter) {
            currentStatus->cap++;
        } else if ((*thisEvent).eventType == Leave) {
            currentStatus->cap--;
        } else if ((*thisEvent).eventType == Reject) {
            currentStatus->rejected++;
        }
    }
    return;
}
