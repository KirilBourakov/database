//
// Created by Kiril on 4/26/2026.
//

#include "page.h"

#include <stdlib.h>
#include <string.h>

struct DbPage {
    char data[PAGE_SIZE];
};

DbPage* create_page(const int id) {
    DbPage* page = calloc(1, sizeof(DbPage));
    PageHeader* header = (PageHeader*)page->data;

    header->page_id = id;
    header->num_slots = 0;
    header->slot_array_end = sizeof(PageHeader);
    header->row_data_start = PAGE_SIZE;
    return page;
}

void destroy_page(DbPage** page_ptr) {
    if (page_ptr != NULL && *page_ptr != NULL) {
        free(*page_ptr);
        *page_ptr = NULL;
    }
}

PageHeader* page_get_header(const DbPage* page) {
    return (PageHeader*)page->data;
}

void* page_get_raw_data(DbPage* page) {
    return page->data;
}

void* slot_data(DbPage* page, const uint16_t slot) {
    const size_t offset = sizeof(PageHeader) + (slot * PAGE_SLOT_SIZE);

    uint16_t rel_loc;
    memcpy(&rel_loc, page->data + offset, sizeof(uint16_t));
    if (rel_loc == 0) {
        return NULL;
    }

    return page->data + rel_loc;
}

int page_insert(DbPage* page, const void* packed_row, const uint16_t row_size) {
    PageHeader* header = (PageHeader*)page->data;

    const size_t space_needed = row_size + PAGE_SLOT_SIZE;
    const int space_free = header->row_data_start - header->slot_array_end;
    if (space_needed > space_free) {
        return -1;
    }

    header->row_data_start -= row_size;
    memcpy(page->data + header->row_data_start, packed_row, row_size);


    memcpy(page->data + header->slot_array_end, &header->row_data_start, PAGE_SLOT_SIZE);
    int id = header->num_slots;
    header->num_slots += 1;
    header->slot_array_end += PAGE_SLOT_SIZE;

    return id;
}
