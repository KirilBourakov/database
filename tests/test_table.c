#include "../lib/unity/unity.h"
#include "../src/table.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void setUp(void) {
    remove("test.data");
}

void tearDown(void) {
    remove("test.data");
}

void test_SingleRowWriteRead(void) {
    ColumnDef defs[] = {
        make_column(TYPE_INT64),
        make_column(TYPE_FIXED_STRING, 4),
        make_column(TYPE_VAR_STRING, 100)
    };
    DbSchema* schema = alloc_schema(defs, 3);

    FILE *fp = fopen("test.data", "wb+");
    TEST_ASSERT_NOT_NULL(fp);

    DbRow *rowIn = malloc_row(schema);
    rowIn->values[0].value.i = 123456789;
    strcpy(rowIn->values[1].value.fixed_string, "ABC");
    
    char* var_val = "Variable Length Unity Test";
    rowIn->values[2].value.var.bytes = strlen(var_val) + 1;
    rowIn->values[2].value.var.data = malloc(rowIn->values[2].value.var.bytes);
    memcpy(rowIn->values[2].value.var.data, var_val, rowIn->values[2].value.var.bytes);

    write_row(schema, fp, rowIn);
    dealloc_row(schema, rowIn);

    rewind(fp);

    DbRow *rowOut = malloc_row(schema);
    size_t read_bytes = read_next_row(schema, fp, rowOut);
    
    TEST_ASSERT_GREATER_THAN(0, read_bytes);
    TEST_ASSERT_EQUAL_INT64(123456789, rowOut->values[0].value.i);
    TEST_ASSERT_EQUAL_STRING("ABC", rowOut->values[1].value.fixed_string);
    TEST_ASSERT_EQUAL_STRING(var_val, rowOut->values[2].value.var.data);
    TEST_ASSERT_EQUAL_INT(strlen(var_val) + 1, rowOut->values[2].value.var.bytes);

    dealloc_row(schema, rowOut);
    fclose(fp);
    dealloc_schema(schema);
}

void test_MultipleRowsAppend(void) {
    ColumnDef defs[] = {
        make_column(TYPE_INT64),
        make_column(TYPE_VAR_STRING, 50)
    };
    DbSchema* schema = alloc_schema(defs, 2);

    FILE *fp = fopen("test.data", "wb+");
    
    for (int i = 0; i < 5; i++) {
        DbRow *row = malloc_row(schema);
        row->values[0].value.i = i;
        char buf[20];
        sprintf(buf, "String %d", i);
        row->values[1].value.var.bytes = strlen(buf) + 1;
        row->values[1].value.var.data = malloc(row->values[1].value.var.bytes);
        memcpy(row->values[1].value.var.data, buf, row->values[1].value.var.bytes);
        
        write_row(schema, fp, row);
        dealloc_row(schema, row);
    }
    fclose(fp);

    fp = fopen("test.data", "rb");
    DbRow *rowOut = malloc_row(schema);
    for (int i = 0; i < 5; i++) {
        size_t read = read_next_row(schema, fp, rowOut);
        TEST_ASSERT_GREATER_THAN(0, read);
        TEST_ASSERT_EQUAL_INT64(i, rowOut->values[0].value.i);
        char buf[20];
        sprintf(buf, "String %d", i);
        TEST_ASSERT_EQUAL_STRING(buf, rowOut->values[1].value.var.data);
    }
    
    // Check EOF
    TEST_ASSERT_EQUAL_INT(0, read_next_row(schema, fp, rowOut));

    dealloc_row(schema, rowOut);
    fclose(fp);
    dealloc_schema(schema);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_SingleRowWriteRead);
    RUN_TEST(test_MultipleRowsAppend);
    return UNITY_END();
}
