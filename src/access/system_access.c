//
// Created by KirilBourakov on 2026-05-14.
//
#include "system_access.h"

#include <stdlib.h>

#include "cursor.h"
#include "file_info.h"

#define ROOT_PAGE_NAME "first_page"
#define BLOB_NAME "defn"

struct SystemAccessor {
    TableCursor* cursor;
    DbSchema* schema;
};

DbSchema* get_table_schema() {
    const ColumnDef cols[] = {
        make_column(TYPE_INT64, ROOT_PAGE_NAME, COL_FLAG_NONE),
        make_column(TYPE_VAR_BLOB, -1, BLOB_NAME, COL_FLAG_NOT_NULLABLE)
    };
    return alloc_schema(cols, sizeof(cols)/sizeof(ColumnDef));
}

SystemAccessor* alloc_system_access(FILE* file) {
    SystemAccessor* access = (SystemAccessor*)malloc(sizeof(SystemAccessor));
    access->schema = get_table_schema();
    access->cursor = start_table_scan(file, 0);
    return access;
}

void dealloc_system_access(SystemAccessor** access) {
    dealloc_schema((*access)->schema);
    stop_table_scan(&(*access)->cursor);
    free(*access);
    *access = NULL;
}

uint64_t insert_new_schema(const SystemAccessor* sys_access, const DbSchema* new_schema) {
    uint64_t page_count = get_page_count(cursor_get_file(sys_access->cursor));
    uint64_t buffer_size;
    void* buffer = alloc_serialize_schema(new_schema, &buffer_size);
    DbRow* row = create_row(sys_access->schema, &page_count, buffer, buffer_size);
    insert(sys_access->cursor, sys_access->schema, row);

    dealloc_row(sys_access->schema, row);
    free(buffer);

    return page_count;
}

DbSchema* get_next_schema(const SystemAccessor* sys_access, uint64_t* schema_root) {
    DbRow* row = malloc_row(sys_access->schema);
    bool value = cursor_next(sys_access->cursor, sys_access->schema, row);

    if (value) {
        const DbValue* val = get_row_value(sys_access->schema, row, BLOB_NAME);
        DbSchema* schema = create_schema_from_packed((uint8_t*) val->value.var.data);

        *schema_root = get_row_value(sys_access->schema, row, ROOT_PAGE_NAME)->value.i;

        dealloc_row(sys_access->schema, row);

        return schema;
    }

    dealloc_row(sys_access->schema, row);
    *schema_root = -1;
    return NULL;
}