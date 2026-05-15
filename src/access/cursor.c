//
// Created by Kiril on 4/26/2026.
//
#include "cursor.h"

#include <stdlib.h>
#include <string.h>

#include "../errors.h"

struct TableCursor {
    FILE* file;
    DbPage* current_page;
    uint16_t current_slot;
    bool end_of_table;
};

FILE* cursor_get_file(const TableCursor* cursor) {
    return cursor->file;
}

TableCursor* start_table_scan(FILE* file, const int page_id) {
    TableCursor* cursor = malloc(sizeof(TableCursor));
    cursor->file = file;
    cursor->current_slot = 0;
    cursor->end_of_table = false;
    cursor->current_page = create_page(page_id);

    fseek(file, page_id*PAGE_SIZE, SEEK_SET);
    if (fread(page_get_raw_data(cursor->current_page), 1, PAGE_SIZE, file) != PAGE_SIZE) {
        cursor->end_of_table = true;
    }

    return cursor;
}

void stop_table_scan(TableCursor** cursor_ptr) {
    if (cursor_ptr != NULL && *cursor_ptr != NULL) {
        destroy_page(&(*cursor_ptr)->current_page);
        free(*cursor_ptr);
        *cursor_ptr = NULL;
    }
}

bool cursor_next(TableCursor* cursor, const DbSchema* schema, const DbRow* out_row) {
    if (cursor->end_of_table) {
        return false;
    }

    while (true) {
        PageHeader* header = page_get_header(cursor->current_page);
        
        if (cursor->current_slot < header->num_slots) {
            const void* raw_row_memory = slot_data(cursor->current_page, cursor->current_slot);
            cursor->current_slot++;

            if (raw_row_memory != NULL) {
                unpack_row(schema, raw_row_memory, out_row);
                return true;
            }
            // If raw_row_memory is NULL, it's a deleted slot; continue to next slot
            continue;
        }

        // Current page exhausted, try moving to the next page
        if (header->next_page_id == INVALID_PAGE_ID) {
            cursor->end_of_table = true;
            return false;
        }

        fseek(cursor->file, (long)header->next_page_id * PAGE_SIZE, SEEK_SET);
        if (fread(page_get_raw_data(cursor->current_page), 1, PAGE_SIZE, cursor->file) != PAGE_SIZE) {
            cursor->end_of_table = true;
            return false;
        }
        
        cursor->current_slot = 0;
    }
}

// temp method
int insert(const TableCursor* cursor, const DbSchema* schema, const DbRow* row) {
    size_t expected_size = row_packed_size(schema, row);

    void* buffer = malloc(expected_size);
    if (buffer != NULL) {
        pack_row(schema, row, buffer);
        const int code = page_insert(cursor->current_page, buffer, expected_size);

        free(buffer);
        return code;
    }
    DIE("write_row failed to alloc buffer.");
}

DbPage* cursor_get_page(TableCursor* cursor) {
    return cursor->current_page;
}
