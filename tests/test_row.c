#include "../lib/unity/unity.h"
#include "../src/model/row.h"
#include <string.h>
#include <stdlib.h>

void test_malloc_dealloc_row(void) {
    ColumnDef defs[] = {
        make_column(TYPE_INT64, "t1"),
        make_column(TYPE_VAR_STRING, 50, "t2")
    };
    DbSchema* schema = alloc_schema(defs, 2);

    DbRow* row = malloc_row(schema);
    TEST_ASSERT_NOT_NULL(row);
    TEST_ASSERT_EQUAL_INT(TYPE_INT64, row->values[0].type);
    TEST_ASSERT_EQUAL_INT(TYPE_VAR_STRING, row->values[1].type);

    dealloc_row(schema, row);
    dealloc_schema(schema);
}

void test_create_row(void) {
    ColumnDef defs[] = {
        make_column(TYPE_INT64, "t1"),
        make_column(TYPE_FIXED_STRING, 10, "t2"),
        make_column(TYPE_VAR_STRING, 50, "t3")
    };
    DbSchema* schema = alloc_schema(defs, 3);

    char* var_data = "Variable Length Data";
    int var_len = strlen(var_data) + 1;
    DbRow* row = create_row(schema, &(int64_t){987654321}, "FixedStr", var_data, var_len);

    TEST_ASSERT_EQUAL_INT64(987654321, row->values[0].value.i);
    TEST_ASSERT_EQUAL_STRING("FixedStr", row->values[1].value.fixed_string);
    TEST_ASSERT_EQUAL_STRING(var_data, row->values[2].value.var.data);
    TEST_ASSERT_EQUAL_INT(var_len, row->values[2].value.var.bytes);

    dealloc_row(schema, row);
    dealloc_schema(schema);
}

void test_pack_unpack_row(void) {
    ColumnDef defs[] = {
        make_column(TYPE_INT64, "t1"),
        make_column(TYPE_FIXED_STRING, 10, "t2"),
        make_column(TYPE_VAR_STRING, 50, "t3")
    };
    DbSchema* schema = alloc_schema(defs, 3);

    char* var_data = "Variable Length Data";
    int var_len = strlen(var_data) + 1;
    DbRow* rowIn = create_row(schema, &(int64_t){123456789}, "Fixed", var_data, var_len);

    // Calculate buffer size
    size_t buffer_size = row_packed_size(schema, rowIn);
    void* buffer = malloc(buffer_size);

    pack_row(schema, rowIn, buffer);

    DbRow* rowOut = malloc_row(schema);
    unpack_row(schema, buffer, rowOut);

    TEST_ASSERT_EQUAL_INT64(123456789, rowOut->values[0].value.i);
    TEST_ASSERT_EQUAL_STRING("Fixed", rowOut->values[1].value.fixed_string);
    TEST_ASSERT_EQUAL_STRING(var_data, rowOut->values[2].value.var.data);
    TEST_ASSERT_EQUAL_INT(var_len, rowOut->values[2].value.var.bytes);

    dealloc_row(schema, rowIn);
    dealloc_row(schema, rowOut);
    free(buffer);
    dealloc_schema(schema);
}
