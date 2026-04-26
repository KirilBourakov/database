//
// Created by Kiril on 4/13/2026.
//

#ifndef DATABASE_TABLE_H
#define DATABASE_TABLE_H
#include <stdint.h>
#include <stdio.h>

#include "data_type.h"
#include "schema.h"

typedef struct {
    DataType type;
    union {
        int64_t i;
        double f;
        char* fixed_string;
        struct {
            char* data;
            size_t bytes;
        } var;
    } value;
} DbValue;

typedef struct {
    DbValue* values;
} DbRow;

DbRow* malloc_row(const DbSchema* schema);
void dealloc_row(const DbSchema* schema, DbRow* buffer);
void unpack_row(const DbSchema* schema, const void* raw_row_memory, const DbRow* out_row);
void pack_row(const DbSchema* schema, const DbRow* row, void* buffer);

#endif //DATABASE_TABLE_H