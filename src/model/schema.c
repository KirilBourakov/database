//
// Created by Kiril on 4/24/2026.
//

#include "schema.h"

#include <stdlib.h>
#include <string.h>

#include "../errors.h"

DbSchema* alloc_schema(const ColumnDef* columns, const size_t count) {
    DbSchema* schema = (DbSchema*) malloc(sizeof(DbSchema));
    if (!schema) return NULL;
    schema->columns_count = count;
    schema->bitmap_bytes = schema->columns_count/8 + (schema->columns_count % 8 != 0);
    schema->offset_bytes = 0;
    schema->fixed_bytes = 0;

    schema->columns = malloc(sizeof(ColumnDef) * count);
    if (!schema->columns) {
        free(schema);
        return NULL;
    }
    for (size_t i = 0; i < count; i++) {
        schema->columns[i] = columns[i];
        schema->columns[i].name = strdup(schema->columns[i].name); // claim ownership
        if (is_variable_size(columns[i].type)) {
            schema->offset_bytes += 2;
        }
        else {
            schema->fixed_bytes += schema->columns[i].bytes;
        }
    }
    return schema;
}

void dealloc_schema(DbSchema* schema) {
    if (schema) {
        if (schema->columns) {
            for (size_t i = 0; i < schema->columns_count; i++) {
                if (schema->columns[i].name) {
                    free((void*)schema->columns[i].name);
                }
            }
            free(schema->columns);
        }
        free(schema);
    }
}

int get_column_index(const DbSchema* schema, const char* name) {
    for (size_t i = 0; i < schema->columns_count; i++) {
        if (strcmp(schema->columns[i].name, name) == 0) {
            return (int)i;
        }
    }
    return -1;
}

ColumnDef make_column_impl(const DataType type, const int explicit_size, const char* name, const uint16_t flags) {
    const int default_size = get_default_size(type);

    if (default_size > 0 && explicit_size != 0) {
        DIE("Explicit size is set in default size column type");
    }
    if (default_size <= 0 && explicit_size <= 0) {
        DIE("Explicit size is less then or equal to 0 in dynamic column");
    }

    const int final_bytes = (default_size > 0) ? default_size : explicit_size;
    return (ColumnDef){ .type = type, .bytes = final_bytes, .name = name, .flags = flags };
}