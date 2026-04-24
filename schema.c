//
// Created by Kiril on 4/24/2026.
//

#include "schema.h"

#include <stdlib.h>

DbSchema* alloc_schema(ColumnDef* columns, size_t count) {
    DbSchema* schema = (DbSchema*) malloc(sizeof(DbSchema));
    schema->columns_count = count;
    schema->columns = malloc(sizeof(ColumnDef) * count);
    for (size_t i = 0; i < count; i++) {
        schema->columns[i] = columns[i];
    }
    return schema;
}

size_t schema_size_bytes(const DbSchema* schema) {
    size_t size = 0;
    for (size_t i = 0; i < schema->columns_count; i++) {
        size += schema->columns[i].size;
    }
    return size;
}
