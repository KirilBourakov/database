//
// Created by Kiril on 4/26/2026.
//

#ifndef DATABASE_CURSOR_H
#define DATABASE_CURSOR_H
#include <stdio.h>

#include "../storage/page.h"
#include "../model/row.h"

typedef struct TableCursor TableCursor;

/**
 * @brief Starts a full table scan on the provided file.
 * 
 * @param file The file containing the table data.
 * @return TableCursor* A pointer to the newly created cursor.
 */
TableCursor* start_table_scan(FILE* file);

/**
 * @brief Stops a table scan and deallocates the cursor.
 * 
 * @param cursor_ptr A pointer to the cursor pointer to deallocate and nullify.
 */
void stop_table_scan(TableCursor** cursor_ptr);

/**
 * @brief Advances the cursor to the next row in the table.
 * 
 * @param cursor The cursor to advance.
 * @param schema The schema defining the row structure.
 * @param out_row The row structure to populate with the next row's data.
 * @return int 0 on success, non-zero on failure or end of table.
 */
int cursor_next(TableCursor* cursor, const DbSchema* schema, const DbRow* out_row);

/**
 * @brief Inserts a new row into the table at the cursor's current position.
 * 
 * @param cursor The cursor indicating the insertion point.
 * @param schema The schema defining the row structure.
 * @param row The row to insert.
 * @return int 0 on success, non-zero on failure.
 */
int insert(const TableCursor* cursor, const DbSchema* schema, const DbRow* row);

/**
 * @brief Retrieves the current page the cursor is pointing to.
 * 
 * @param cursor The cursor to query.
 * @return DbPage* A pointer to the current page.
 */
DbPage* cursor_get_page(TableCursor* cursor);

#endif //DATABASE_CURSOR_H