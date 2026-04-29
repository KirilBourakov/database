//
// Created by Kiril on 4/24/2026.
//

#ifndef DATABASE_SCHEMA_H
#define DATABASE_SCHEMA_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "data_type.h"


typedef enum {
    COL_FLAG_NONE = 0b0,
    COL_FLAG_PK = 0b1,
    COL_FLAG_NOT_NULLABLE = 0b10
} ColumnFlags;

typedef struct {
    DataType type;
    int bytes;
    uint16_t flags;
    const char* name;
} ColumnDef;

/**
 * @brief Internal implementation for creating a column definition.
 * 
 * This function is used by the make_column macro to handle both 3 and 4 argument calls.
 * 
 * @param type The data type of the column.
 * @param explicit_size The size of the column in bytes (for FIXED_STRING).
 * @param name The name of the column.
 * @param flags Flags for the column (e.g., PK, NOT_NULLABLE).
 * @return ColumnDef The created column definition.
 */
ColumnDef make_column_impl(DataType type, int explicit_size, const char* name, uint16_t flags);

/* Support:
   make_column(type, name, flags) -> 3 args
   make_column(type, size, name, flags) -> 4 args
*/
#define MAKE_COL_3(type, name, flags) \
    make_column_impl(type, 0, name, flags)
#define MAKE_COL_4(type, size, name, flags) \
    make_column_impl(type, size, name, flags)

#define GET_MACRO(_1, _2, _3, _4, NAME, ...) NAME

/**
 * @brief Macro for creating a column definition with either 3 or 4 arguments.
 * 
 * Usage:
 * make_column(type, name, flags)
 * make_column(type, size, name, flags)
 */
#define make_column(...) GET_MACRO(__VA_ARGS__, MAKE_COL_4, MAKE_COL_3)(__VA_ARGS__)

typedef struct {
    ColumnDef* columns;
    size_t columns_count;

    size_t bitmap_bytes;
    size_t offset_bytes;
    size_t fixed_bytes;
} DbSchema;

/**
 * @brief Allocates and initializes a DbSchema from an array of column definitions.
 * 
 * @param columns An array of column definitions.
 * @param count The number of columns in the array.
 * @return DbSchema* A pointer to the newly allocated schema.
 */
DbSchema* alloc_schema(const ColumnDef* columns, size_t count);

/**
 * @brief Deallocates memory used by a DbSchema.
 * 
 * @param schema The schema to deallocate.
 */
void dealloc_schema(DbSchema* schema);

/**
 * @brief Retrieves the index of a column by its name.
 * 
 * @param schema The schema to search.
 * @param name The name of the column.
 * @return int The index of the column, or -1 if not found.
 */
int get_column_index(const DbSchema* schema, const char* name);

#endif //DATABASE_SCHEMA_H