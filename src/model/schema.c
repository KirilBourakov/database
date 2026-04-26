//
// Created by Kiril on 4/24/2026.
//

#include "schema.h"

#include <stdlib.h>

#include "../errors.h"

DbSchema* alloc_schema(ColumnDef* columns, size_t count) {
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
        if (schema->columns) free(schema->columns);
        free(schema);
    }
}

ColumnDef make_column_impl(const DataType type, const int explicit_size) {
    const int default_size = get_default_size(type);
    if (default_size > 0 && explicit_size == 0) {
        return (ColumnDef){ .type = type, .bytes = default_size };
    }
    if (default_size > 0) {
        DIE("Explicit size is set in default size column type");
    }
    if (explicit_size <= 0) {
        DIE("Explicit size is less then or equal to 0 in dynamic column");
    }
    return (ColumnDef){ .type = type, .bytes = explicit_size };
}