//
// Created by Kiril on 4/26/2026.
//

#ifndef DATABASE_PAGE_H
#define DATABASE_PAGE_H

#define PAGE_SIZE 8192
#define PAGE_SLOT_SIZE sizeof(uint16_t)

#include <stdint.h>

typedef struct {
    uint32_t page_id;

    uint16_t slot_array_end;
    uint16_t row_data_start;
    uint16_t num_slots;
} PageHeader;

typedef struct DbPage DbPage;

DbPage* create_page(int id);
void destroy_page(DbPage** page_ptr);

PageHeader* page_get_header(const DbPage* page);
void* page_get_raw_data(DbPage* page);

void* slot_data(DbPage* page, uint16_t slot);
int page_insert(DbPage* page, const void* packed_row, uint16_t row_size);

#endif //DATABASE_PAGE_H