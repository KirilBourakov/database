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
        struct {
            char* value;
        } fixed_string;
    } value;
} DbValue;

typedef struct {
    DbValue* values;
} DbRow;

size_t read_next_row(const DbSchema *schema, FILE *fp, DbRow *buffer);
DbRow* malloc_row(const DbSchema* schema);
void dealloc_row(DbRow* buffer);
void write_row(const DbSchema* schema, FILE *fp, const DbRow* row);

#endif //DATABASE_TABLE_H