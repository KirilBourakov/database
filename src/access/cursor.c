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
    int end_of_table;
};

TableCursor* start_table_scan(FILE* file) {
    TableCursor* cursor = malloc(sizeof(TableCursor));
    cursor->file = file;
    cursor->current_slot = 0;
    cursor->end_of_table = 0;
    cursor->current_page = create_page(0);

    rewind(file);
    if (fread(page_get_raw_data(cursor->current_page), 1, PAGE_SIZE, file) != PAGE_SIZE) {
        cursor->end_of_table = 1;
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

int cursor_next(TableCursor* cursor, const DbSchema* schema, const DbRow* out_row) {
    if (cursor->end_of_table) {
        return 0;
    }

    PageHeader* header = page_get_header(cursor->current_page);
    while (1) {
        if (cursor->current_slot >= header->num_slots) {
            // Read the next physical page in the file
            if (fread(page_get_raw_data(cursor->current_page), 1, PAGE_SIZE, cursor->file) != PAGE_SIZE) {
                cursor->end_of_table = 1;
                return 0;
            }

            cursor->current_slot = 0;
            header = page_get_header(cursor->current_page);
            continue;
        }

        const void* raw_row_memory = slot_data(cursor->current_page, cursor->current_slot);
        cursor->current_slot++;

        // If raw_row_memory is NULL, it means this specific slot was deleted.
        if (raw_row_memory != NULL) {
            unpack_row(schema, raw_row_memory, out_row);
            return 1;
        }
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
