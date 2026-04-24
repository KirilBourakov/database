//
// Created by Kiril on 4/13/2026.
//

#include "table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DbRow* malloc_row(const DbSchema* schema) {
    DbRow* buffer = malloc(sizeof(DbRow));
    buffer->values = malloc(sizeof(DbValue) * schema->columns_count);
    for (size_t i = 0; i < schema->columns_count; i++) {
        buffer->values[i].type = schema->columns[i].type;
        if (schema->columns[i].type == TYPE_FIXED_STRING) {
            buffer->values[i].value.fixed_string = malloc(schema->columns[i].size);
        }
    }
    return buffer;
}
void dealloc_row(const DbSchema* schema, DbRow* buffer) {
    for (size_t i = 0; i < schema->columns_count; i++) {
        if (schema->columns[i].type == TYPE_FIXED_STRING) {
            free(buffer->values[i].value.fixed_string);
        }
    }
    free(buffer->values);
    free(buffer);
}

void unpack_row(const DbSchema* schema, const DbRow* row, const void* buffer) {
    size_t consumed = 0;
    const char* ptr = (const char*)buffer;
    for (size_t i = 0; i < schema->columns_count; i++) {
        switch (schema->columns[i].type) {
            case (TYPE_INT64):
                memcpy(&row->values[i].value.i, ptr + consumed, schema->columns[i].size);
                break;
            case (TYPE_DOUBLE):
                memcpy(&row->values[i].value.f, ptr + consumed, schema->columns[i].size);
                break;
            case (TYPE_FIXED_STRING):
                memcpy(row->values[i].value.fixed_string, ptr + consumed, schema->columns[i].size);
                break;
        }
        consumed += schema->columns[i].size;
    }
}

size_t read_next_row(const DbSchema *schema, FILE *fp, const DbRow *buffer) {
    const size_t expected_buffer_size = schema_size_bytes(schema);

    void* tmp = malloc(expected_buffer_size);
    if (tmp != NULL) {
        const size_t read = fread(tmp, 1, expected_buffer_size, fp);
        if (read == expected_buffer_size) {
            unpack_row(schema, buffer, tmp);
        }

        free(tmp);
        return read;
    }
    perror("read_next_row failed to alloc buffer.");
    return 0;
}

void pack_row(const DbSchema* schema, const DbRow* row, void* buffer) {
    size_t consumed = 0;
    char* ptr = (char*)buffer;
    for (size_t i = 0; i < schema->columns_count; i++) {
        switch (schema->columns[i].type) {
            case (TYPE_INT64):
                memcpy(ptr + consumed, &row->values[i].value.i, schema->columns[i].size);
                break;

            case (TYPE_DOUBLE):
                memcpy(ptr + consumed, &row->values[i].value.f, schema->columns[i].size);
                break;

            case (TYPE_FIXED_STRING):
                memcpy(ptr + consumed, row->values[i].value.fixed_string, schema->columns[i].size);
                break;
        }
        consumed += schema->columns[i].size;
    }
}
void write_row(const DbSchema* schema, FILE *fp, const DbRow* row) {
    const size_t expected_buffer_size = schema_size_bytes(schema);
    void* buffer = malloc(expected_buffer_size);
    if (buffer != NULL) {
        pack_row(schema, row, buffer);
        fwrite(buffer, 1, expected_buffer_size, fp);
        free(buffer);
    }
    else {
        perror("write_row failed to alloc buffer.");
    }
}
