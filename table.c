//
// Created by Kiril on 4/13/2026.
//

#include "table.h"

#include <stdio.h>
#include <stdlib.h>

DbRow* malloc_row(const DbSchema* schema) {
    DbRow* buffer = malloc(sizeof(DbRow));
    buffer->values = malloc(schema_size_bytes(schema));
    return buffer;
}
void dealloc_row(DbRow* buffer) {
    free(buffer->values);
    free(buffer);
}

size_t read_next_row(const DbSchema *schema, FILE *fp, DbRow *buffer) {
    const size_t expected_buffer_size = schema_size_bytes(schema);
    return fread(buffer->values, 1, expected_buffer_size, fp);
}

void write_row(const DbSchema* schema, FILE *fp, const DbRow* row) {
    const size_t expected_buffer_size = schema_size_bytes(schema);
    fwrite(row->values, 1, expected_buffer_size, fp);
}
