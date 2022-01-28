#include "hardware/flash.h"
#include "hardware/sync.h"
#include <stdlib.h>
#include <string.h>
#include "structs.h"

#ifndef INCLUDE_WRITER_H
#define INCLUDE_WRITER_H

#define TOTAL_FLASH_SIZE 2097152

// This variable is automatically assigned the memory location
// of the end of the program on the flash storage. I.E. it
// signifies the first place that's safe to write.
// extern char* __flash_binary_end;

// This stores the current page number that the program should
// write to, starting from 0 at the end of the program itself.
// unsigned int currentPage;

// This stores the first safe write location, as an offset into
// the flash memory rather than an absolute address.
// int writeStartLoc;

void writePage(struct Event eventToWrite);

struct Event *readPage(int pageNumber);

// int findCurrentPage(int start, int end);

void clear_flash();

void writer_init(struct Status *currentStatus);

#endif
