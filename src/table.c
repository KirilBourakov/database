//
// Created by Kiril on 4/13/2026.
//

#include "table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

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

void unpack_row(const DbSchema* schema, DbRow* row, const void* buffer) {
    size_t consumed = 0;
    const char* ptr = (const char*)buffer;

    // Skip Null Bitmap (not used yet)
    consumed += schema->bitmap_bytes;

    // Offsets
    const uint16_t* offsets = (const uint16_t*)(ptr + consumed);
    consumed += schema->offset_bytes;

    // Fixed length chunk
    for (size_t i = 0; i < schema->columns_count; i++) {
        if (!is_variable_size(schema->columns[i].type)) {
            if (schema->columns[i].type == TYPE_FIXED_STRING) {
                memcpy(row->values[i].value.fixed_string, ptr + consumed, schema->columns[i].bytes);
            } else {
                memcpy(&row->values[i].value, ptr + consumed, schema->columns[i].bytes);
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

            if (row->values[i].value.var.data) {
                free(row->values[i].value.var.data);
            }
            row->values[i].value.var.data = malloc(bytes);
            memcpy(row->values[i].value.var.data, ptr + consumed, bytes);
            row->values[i].value.var.bytes = bytes;

            consumed += bytes;
            prev_offset = current_offset;
            var_col_idx++;
        }
    }
}

size_t read_next_row(const DbSchema *schema, FILE *fp, DbRow *buffer) {
    const size_t fixed_part_size = schema->bitmap_bytes + schema->offset_bytes + schema->fixed_bytes;

    char* tmp = malloc(fixed_part_size);
    if (!tmp) {
        DIE("read_next_row failed to alloc buffer.");
    }

    if (fread(tmp, 1, fixed_part_size, fp) != fixed_part_size) {
        free(tmp);
        return 0;
    }

    size_t total_row_size = fixed_part_size;
    char* full_buffer = tmp;

    if (schema->offset_bytes > 0) {
        uint16_t total_var_size;
        memcpy(&total_var_size, tmp + schema->bitmap_bytes + schema->offset_bytes - 2, sizeof(uint16_t));

        if (total_var_size > 0) {
            full_buffer = realloc(tmp, fixed_part_size + total_var_size);
            if (!full_buffer) {
                free(tmp);
                return 0;
            }
            if (fread(full_buffer + fixed_part_size, 1, total_var_size, fp) != total_var_size) {
                free(full_buffer);
                return 0;
            }
            total_row_size += total_var_size;
        }
    }

    unpack_row(schema, buffer, full_buffer);
    free(full_buffer);
    return total_row_size;
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

void write_row(const DbSchema* schema, FILE *fp, const DbRow* row) {
    size_t expected_size = schema->bitmap_bytes + schema->offset_bytes + schema->fixed_bytes;
    for (size_t i = 0; i < schema->columns_count; i++) {
        if (is_variable_size(schema->columns[i].type)) {
            expected_size += row->values[i].value.var.bytes;
        }
    }

    void* buffer = malloc(expected_size);
    if (buffer != NULL) {
        pack_row(schema, row, buffer);
        fwrite(buffer, 1, expected_size, fp);
        free(buffer);
    }
    else {
        DIE("write_row failed to alloc buffer.");
    }
}
