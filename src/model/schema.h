//
// Created by Kiril on 4/24/2026.
//

#ifndef DATABASE_SCHEMA_H
#define DATABASE_SCHEMA_H
#include <stdbool.h>
#include <stddef.h>

#include "data_type.h"


typedef struct {
    DataType type;
    int bytes;
    const char* name;
} ColumnDef;
ColumnDef make_column_impl(DataType type, int explicit_size, const char* name);
#define MAKE_COL_1(type, name) make_column_impl(type, 0, name)
#define MAKE_COL_2(type, size, name) make_column_impl(type, size, name)
#define GET_IMP(arg1, arg2, arg3, arg4, ...) arg4
#define make_column(...) GET_IMP(__VA_ARGS__, MAKE_COL_2, MAKE_COL_1)(__VA_ARGS__)

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