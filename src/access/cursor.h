//
// Created by Kiril on 4/26/2026.
//

#ifndef DATABASE_CURSOR_H
#define DATABASE_CURSOR_H
#include <stdio.h>

#include "../storage/page.h"
#include "../model/row.h"

typedef struct {
    FILE* file;
    DbPage* current_page;
    uint16_t current_slot;
    int end_of_table;
} TableCursor;

TableCursor start_table_scan(FILE* file);
int cursor_next(TableCursor* cursor, const DbSchema* schema, const DbRow* out_row);
int insert(const TableCursor* cursor, const DbSchema* schema, const DbRow* row);

#endif //DATABASE_CURSOR_H