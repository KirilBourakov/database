#include "../lib/unity/unity.h"
#include "../src/model/row.h"
#include <string.h>
#include <stdlib.h>

void test_malloc_dealloc_row(void) {
    ColumnDef defs[] = {
        make_column(TYPE_INT64, "t1", COL_FLAG_NONE),
        make_column(TYPE_VAR_STRING, 50, "t2", COL_FLAG_NONE)
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
        make_column(TYPE_INT64, "t1", COL_FLAG_NONE),
        make_column(TYPE_FIXED_STRING, 10, "t2", COL_FLAG_NONE),
        make_column(TYPE_VAR_STRING, 50, "t3", COL_FLAG_NONE)
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
        make_column(TYPE_INT64, "t1", COL_FLAG_NONE),
        make_column(TYPE_FIXED_STRING, 10, "t2", COL_FLAG_NONE),
        make_column(TYPE_VAR_STRING, 50, "t3", COL_FLAG_NONE)
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

void test_row_contains(void) {
    ColumnDef defs[] = {
        make_column(TYPE_INT64, "pk", COL_FLAG_PK),
        make_column(TYPE_FIXED_STRING, 10, "fixed", COL_FLAG_NONE),
        make_column(TYPE_VAR_STRING, 50, "var", COL_FLAG_NONE)
    };
    DbSchema* schema = alloc_schema(defs, 3);

    char* var_data = "Hello World";
    int var_len = strlen(var_data) + 1;
    int64_t pk_val = 42;
    DbRow* row = create_row(schema, &pk_val, "FixedData", var_data, var_len);

    // Test 1: Match single LITERAL column
    MatchCondition cond1 = {"pk", &pk_val, sizeof(int64_t)};
    TEST_ASSERT_TRUE(row_contains(schema, row, &cond1, 1));

    // Test 2: Match multiple columns
    MatchCondition cond2[] = {
        {"pk", &pk_val, sizeof(int64_t)},
        {"fixed", "FixedData", 10}
    };
    TEST_ASSERT_TRUE(row_contains(schema, row, cond2, 2));

    // Test 3: Match variable length column
    MatchCondition cond3 = {"var", var_data, var_len};
    TEST_ASSERT_TRUE(row_contains(schema, row, &cond3, 1));

    // Test 4: Mismatch on value
    int64_t wrong_pk = 99;
    MatchCondition cond4 = {"pk", &wrong_pk, sizeof(int64_t)};
    TEST_ASSERT_FALSE(row_contains(schema, row, &cond4, 1));

    // Test 5: Mismatch on size
    MatchCondition cond5 = {"var", var_data, var_len - 1};
    TEST_ASSERT_FALSE(row_contains(schema, row, &cond5, 1));

    // Test 6: Non-existent column
    MatchCondition cond6 = {"missing", &pk_val, sizeof(int64_t)};
    TEST_ASSERT_FALSE(row_contains(schema, row, &cond6, 1));

    dealloc_row(schema, row);
    dealloc_schema(schema);
}
