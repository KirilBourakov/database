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
    int size;
} ColumnDef;
ColumnDef make_column_impl(DataType type, int explicit_size);
#define MAKE_COL_1(type) make_column_impl(type, 0)
#define MAKE_COL_2(type, size) make_column_impl(type, size)
#define GET_3RD_ARG(arg1, arg2, arg3, ...) arg3
#define make_column(...) GET_3RD_ARG(__VA_ARGS__, MAKE_COL_2, MAKE_COL_1)(__VA_ARGS__)

typedef struct {
    ColumnDef* columns;
    size_t columns_count;
} DbSchema;

DbSchema* alloc_schema(ColumnDef* columns, size_t count);
size_t schema_size_bytes(const DbSchema* schema);

#endif //DATABASE_SCHEMA_H