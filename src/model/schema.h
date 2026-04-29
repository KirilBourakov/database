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
#define make_column(...) GET_MACRO(__VA_ARGS__, MAKE_COL_4, MAKE_COL_3)(__VA_ARGS__)

typedef struct {
    ColumnDef* columns;
    size_t columns_count;

    size_t bitmap_bytes;
    size_t offset_bytes;
    size_t fixed_bytes;
} DbSchema;

DbSchema* alloc_schema(const ColumnDef* columns, size_t count);
void dealloc_schema(DbSchema* schema);
int get_column_index(const DbSchema* schema, const char* name);

#endif //DATABASE_SCHEMA_H