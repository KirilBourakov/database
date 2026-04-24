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

typedef struct {
    ColumnDef* columns;
    size_t columns_count;
} DbSchema;

DbSchema* alloc_schema(size_t columns_count);
size_t schema_size_bytes(const DbSchema* schema);

#endif //DATABASE_SCHEMA_H