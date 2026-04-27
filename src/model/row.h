//
// Created by Kiril on 4/13/2026.
//

#ifndef DATABASE_ROW_H
#define DATABASE_ROW_H
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

DbRow* create_row(const DbSchema* schema, ...);
DbRow* malloc_row(const DbSchema* schema);
void dealloc_row(const DbSchema* schema, DbRow* buffer);
void unpack_row(const DbSchema* schema, const void* raw_row_memory, const DbRow* out_row);
void pack_row(const DbSchema* schema, const DbRow* row, void* buffer);
size_t row_packed_size(const DbSchema* schema, const DbRow* row);

#endif //DATABASE_ROW_H