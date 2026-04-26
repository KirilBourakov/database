//
// Created by Kiril on 4/26/2026.
//

#include "disk.h"

int writeback(const DbPage* page, FILE* file) {
    const PageHeader* header = (PageHeader*)page->data;
    const uint32_t page_id = header->page_id;
    const long file_offset = (long)page_id * PAGE_SIZE;

    if (fseek(file, file_offset, SEEK_SET) != 0) {
        perror("Failed to seek to page location");
        return 0;
    }

    if (fwrite(page->data, 1, PAGE_SIZE, file) != PAGE_SIZE) {
        perror("Failed to write page to disk");
        return 0;
    }

    if (fflush(file) != 0) {
        perror("Failed to flush to disk");
        return 0;
    }

    return 1;
}
