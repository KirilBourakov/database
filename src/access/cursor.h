//
// Created by Kiril on 4/26/2026.
//

#ifndef DATABASE_CURSOR_H
#define DATABASE_CURSOR_H
#include <stdio.h>

#include "../storage/page.h"
#include "../model/row.h"

typedef struct TableCursor TableCursor;

TableCursor* start_table_scan(FILE* file);
void stop_table_scan(TableCursor** cursor_ptr);

int cursor_next(TableCursor* cursor, const DbSchema* schema, const DbRow* out_row);
int insert(const TableCursor* cursor, const DbSchema* schema, const DbRow* row);

DbPage* cursor_get_page(TableCursor* cursor);

#endif //DATABASE_CURSOR_H