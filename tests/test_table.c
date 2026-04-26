#include "../lib/unity/unity.h"
#include "../src/table.h"
#include <string.h>
#include <stdlib.h>

void test_malloc_dealloc_row(void) {
    ColumnDef defs[] = {
        make_column(TYPE_INT64),
        make_column(TYPE_VAR_STRING, 50)
    };
    DbSchema* schema = alloc_schema(defs, 2);

    DbRow* row = malloc_row(schema);
    TEST_ASSERT_NOT_NULL(row);
    TEST_ASSERT_EQUAL_INT(TYPE_INT64, row->values[0].type);
    TEST_ASSERT_EQUAL_INT(TYPE_VAR_STRING, row->values[1].type);

    dealloc_row(schema, row);
    dealloc_schema(schema);
}

void test_pack_unpack_row(void) {
    ColumnDef defs[] = {
        make_column(TYPE_INT64),
        make_column(TYPE_FIXED_STRING, 10),
        make_column(TYPE_VAR_STRING, 50)
    };
    DbSchema* schema = alloc_schema(defs, 3);

    DbRow* rowIn = malloc_row(schema);
    rowIn->values[0].value.i = 123456789;
    strcpy(rowIn->values[1].value.fixed_string, "Fixed");
    
    char* var_data = "Variable Length Data";
    rowIn->values[2].value.var.bytes = strlen(var_data) + 1;
    rowIn->values[2].value.var.data = malloc(rowIn->values[2].value.var.bytes);
    memcpy(rowIn->values[2].value.var.data, var_data, rowIn->values[2].value.var.bytes);

    // Calculate buffer size: bitmap + offsets + fixed + var
    size_t buffer_size = schema->bitmap_bytes + schema->offset_bytes + schema->fixed_bytes + rowIn->values[2].value.var.bytes;
    void* buffer = malloc(buffer_size);

    pack_row(schema, rowIn, buffer);

    DbRow* rowOut = malloc_row(schema);
    unpack_row(schema, buffer, rowOut);

    TEST_ASSERT_EQUAL_INT64(123456789, rowOut->values[0].value.i);
    TEST_ASSERT_EQUAL_STRING("Fixed", rowOut->values[1].value.fixed_string);
    TEST_ASSERT_EQUAL_STRING(var_data, rowOut->values[2].value.var.data);
    TEST_ASSERT_EQUAL_INT(strlen(var_data) + 1, rowOut->values[2].value.var.bytes);

    dealloc_row(schema, rowIn);
    dealloc_row(schema, rowOut);
    free(buffer);
    dealloc_schema(schema);
}
