//
// Created by Kiril on 4/26/2026.
//

#ifndef DATABASE_CURSOR_H
#define DATABASE_CURSOR_H
#include <stdio.h>

#include "../storage/page.h"
#include "../model/row.h"

typedef struct TableCursor TableCursor;

FILE* cursor_get_file(const TableCursor* cursor);

/**
 * @brief Allocates and initializes a table cursor for a full table scan.
 * 
 * @param file The file containing the table data.
 * @param page_id Root page id for the schema.
 * @return TableCursor* A pointer to the newly allocated cursor.
 */
TableCursor* alloc_table_cursor(FILE* file, int page_id);

/**
 * @brief Deallocates memory used by a table cursor and nullifies the pointer.
 * 
 * @param cursor_ptr A pointer to the cursor pointer to deallocate and nullify.
 */
void dealloc_table_cursor(TableCursor** cursor_ptr);

/**
 * @brief Advances the cursor to the next row in the table.
 * 
 * @param cursor The cursor to advance.
 * @param schema The schema defining the row structure.
 * @param out_row The row structure to populate with the next row's data.
 * @return int 0 on success, non-zero on failure or end of table.
 */
bool cursor_next(TableCursor* cursor, const DbSchema* schema, const DbRow* out_row);

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
DbPage* cursor_get_page(const TableCursor* cursor);

#endif //DATABASE_CURSOR_H