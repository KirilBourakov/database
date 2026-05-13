//
// Created by Kiril on 4/24/2026.
//

#include "schema.h"

#include <stdlib.h>
#include <string.h>

#include "../errors.h"
#include "storage/page.h"

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

DbSchema* create_schema_from_packed(uint8_t* data) {
    #define READ_FIELD(val) do { \
        memcpy(&(val), data, sizeof(val)); \
        data += sizeof(val); \
    } while(0)

    uint64_t columns_count;
    READ_FIELD(columns_count);

    ColumnDef* cols = malloc(columns_count * sizeof(ColumnDef));
    if (!cols) {
        DIE("Allocation failed for columns");
    }
    for (size_t i = 0; i < columns_count; i++) {
        uint32_t type_val;
        READ_FIELD(type_val);
        cols[i].type = (DataType)type_val;
        READ_FIELD(cols[i].bytes);
        READ_FIELD(cols[i].flags);
    }


    for (size_t i = 0; i < columns_count; i++) {
        cols[i].name = strdup((char*) data);
        data += strlen(cols[i].name) + 1;
    }

    #undef READ_FIELD

    DbSchema* nv = alloc_schema(cols, columns_count);
    for (size_t i = 0; i < columns_count; i++) {
        free((void*)cols[i].name);
    }
    free(cols);
    return nv;
}

/**
 * Serialize a provided schema into the format [Col Count][Column Data][Column Names]
 * @param schema The database schema
 * @param size_out The size of the returned pointer
 * @return The allocated memory, filled with the packed schema
 */
void* alloc_serialize_schema(const DbSchema* schema, size_t* size_out) {
    size_t size = 0;
    size += sizeof(schema->columns_count);
    // Column fields and strings
    for (size_t i = 0; i < schema->columns_count; i++) {
        size += sizeof(schema->columns[i].type);
        size += sizeof(schema->columns[i].bytes);
        size += sizeof(schema->columns[i].flags);
        size += strlen(schema->columns[i].name) + 1;
    }

    uint8_t* data = malloc(size);
    if (!data) {
        DIE("Allocation failed");
    }
    uint8_t* ptr = data;

    // Helper macro to keep the code clean and prevent typos
    #define WRITE_FIELD(val) do { \
        memcpy(ptr, &(val), sizeof(val)); \
        ptr += sizeof(val); \
    } while(0)

    WRITE_FIELD(schema->columns_count);

    // write columns
    for (size_t i = 0; i < schema->columns_count; i++) {
        const ColumnDef* col = &schema->columns[i];
        uint32_t type_val = (uint32_t)col->type;
        WRITE_FIELD(type_val);
        WRITE_FIELD(col->bytes);
        WRITE_FIELD(col->flags);
    }
    for (size_t i = 0; i < schema->columns_count; i++) {
        size_t name_len = strlen(schema->columns[i].name) + 1;
        memcpy(ptr, schema->columns[i].name, name_len);
        ptr += name_len;
    }

    #undef WRITE_FIELD

    *size_out = size;
    return data;
}

DbSchema* get_table_schema() {
    const ColumnDef cols[] = {
        make_column(TYPE_INT64, "first_page", COL_FLAG_NONE),
        make_column(TYPE_VAR_BLOB, -1, "defn", COL_FLAG_NOT_NULLABLE)
    };
    return alloc_schema(cols, sizeof(cols)/sizeof(ColumnDef));
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
    const DataTypeClass class = get_variable_typeclass(type);

    if (default_size > 0 && explicit_size != 0) {
        DIE("Explicit size is set in default size column type");
    }

    if (class == FIXED_POINTER && explicit_size <= 0) {
        DIE("Explicit size must be positive for FIXED_STRING");
    }

    if (class == VARIABLE_POINTER && explicit_size <= 0 && explicit_size != -1) {
        DIE("Explicit size must be positive or -1 for variable length types");
    }

    const int final_bytes = (default_size > 0) ? default_size : explicit_size;
    return (ColumnDef){ .type = type, .bytes = final_bytes, .name = name, .flags = flags };
}