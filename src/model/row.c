//
// Created by Kiril on 4/13/2026.
//


#include "row.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../storage/page.h"
#include "../errors.h"

#include <stdarg.h>

DbRow* create_row(const DbSchema* schema, ...) {
    va_list args;
    va_start(args, schema);

    DbRow* row = malloc_row(schema);
    for (size_t i = 0; i < schema->columns_count; i++) {
        const DataTypeClass class = get_variable_typeclass(schema->columns[i].type);
        const void* incoming_data = va_arg(args, void*);
        switch (class) {
            case LITERAL:
                memcpy(&row->values[i].value, incoming_data, schema->columns[i].bytes);
                break;
            case FIXED_POINTER:
                memcpy(row->values[i].value.fixed_string, incoming_data, schema->columns[i].bytes);
                break;
            case VARIABLE_POINTER: {
                const int bytes = va_arg(args, int);
                row->values[i].value.var.data = malloc(bytes);
                memcpy(row->values[i].value.var.data, incoming_data, bytes);
                row->values[i].value.var.bytes = bytes;
                break;
            }
        }
    }

    va_end(args);
    return row;
}

DbRow* malloc_row(const DbSchema* schema) {
    DbRow* buffer = malloc(sizeof(DbRow));
    buffer->values = malloc(sizeof(DbValue) * schema->columns_count);
    for (size_t i = 0; i < schema->columns_count; i++) {
        buffer->values[i].type = schema->columns[i].type;
        if (schema->columns[i].type == TYPE_FIXED_STRING) {
            buffer->values[i].value.fixed_string = malloc(schema->columns[i].bytes);
        } else if (is_variable_size(schema->columns[i].type)) {
            buffer->values[i].value.var.data = NULL;
            buffer->values[i].value.var.bytes = 0;
        }
    }
    return buffer;
}
void dealloc_row(const DbSchema* schema, DbRow* buffer) {
    for (size_t i = 0; i < schema->columns_count; i++) {
        if (schema->columns[i].type == TYPE_FIXED_STRING) {
            free(buffer->values[i].value.fixed_string);
        }
        else if (is_variable_size(schema->columns[i].type)) {
            if (buffer->values[i].value.var.data) {
                free(buffer->values[i].value.var.data);
            }
        }
    }
    free(buffer->values);
    free(buffer);
}

/**
 * Unpack Row
 * @param schema Schema for this table
 * @param raw_row_memory a pointer, read from a db slot
 * @param out_row a pointer to space to parse the row memory
 */
void unpack_row(const DbSchema* schema, const void* raw_row_memory, const DbRow* out_row) {
    size_t consumed = 0;
    const char* ptr = (const char*)raw_row_memory;

    consumed += schema->bitmap_bytes;

    const uint16_t* offsets = (const uint16_t*)(ptr + consumed);
    consumed += schema->offset_bytes;

    for (size_t i = 0; i < schema->columns_count; i++) {
        if (!is_variable_size(schema->columns[i].type)) {
            if (schema->columns[i].type == TYPE_FIXED_STRING) {
                memcpy(out_row->values[i].value.fixed_string, ptr + consumed, schema->columns[i].bytes);
            } else {
                memcpy(&out_row->values[i].value, ptr + consumed, schema->columns[i].bytes);
            }
            consumed += schema->columns[i].bytes;
        }
    }

    // Var length data
    size_t var_col_idx = 0;
    uint16_t prev_offset = 0;
    for (size_t i = 0; i < schema->columns_count; i++) {
        if (is_variable_size(schema->columns[i].type)) {
            uint16_t current_offset = offsets[var_col_idx];
            size_t bytes = current_offset - prev_offset;

            if (out_row->values[i].value.var.data) {
                free(out_row->values[i].value.var.data);
            }
            out_row->values[i].value.var.data = malloc(bytes);
            memcpy(out_row->values[i].value.var.data, ptr + consumed, bytes);
            out_row->values[i].value.var.bytes = bytes;

            consumed += bytes;
            prev_offset = current_offset;
            var_col_idx++;
        }
    }
}

// Pack Row into Null Bitmap | Offsets | Fixed Data | Var Data
void pack_row(const DbSchema* schema, const DbRow* row, void* buffer) {
    size_t consumed = 0;
    char* ptr = (char*)buffer;

    // null bitmap (null currently not supported, to be added)
    memset(ptr, 0, schema->bitmap_bytes);
    consumed += schema->bitmap_bytes;

    // var length offsets
    uint16_t prev = 0;
    for (size_t i = 0; i < schema->columns_count; i++) {
        if (is_variable_size(schema->columns[i].type)) {
            uint16_t size = prev + row->values[i].value.var.bytes;
            memcpy(ptr + consumed, &size, sizeof(uint16_t));
            consumed += sizeof(uint16_t);
            prev = size;
        }
    }

    // fixed length chunk
    for (size_t i = 0; i < schema->columns_count; i++) {
        if (!is_variable_size(schema->columns[i].type)) {
            if (schema->columns[i].type == TYPE_FIXED_STRING) {
                memcpy(ptr + consumed, row->values[i].value.fixed_string, schema->columns[i].bytes);
            } else {
                memcpy(ptr + consumed, &row->values[i].value, schema->columns[i].bytes);
            }
            consumed += schema->columns[i].bytes;
        }
    }

    // var length data
    for (size_t i = 0; i < schema->columns_count; i++) {
        if (is_variable_size(schema->columns[i].type)) {
            memcpy(ptr + consumed, row->values[i].value.var.data, row->values[i].value.var.bytes);
            consumed += row->values[i].value.var.bytes;
        }
    }
}

size_t row_packed_size(const DbSchema* schema, const DbRow* row) {
    size_t size = schema->bitmap_bytes + schema->offset_bytes + schema->fixed_bytes;
    for (size_t i = 0; i < schema->columns_count; i++) {
        if (is_variable_size(schema->columns[i].type)) {
            size += row->values[i].value.var.bytes;
        }
    }
    return size;
}