#include "../lib/unity/unity.h"
#include "../src/access/cursor.h"
#include "../src/model/row.h"
#include "../src/storage/disk.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void test_cursor_lifecycle(void) {
    FILE* fp = fopen("cursor_test.data", "wb+");
    TEST_ASSERT_NOT_NULL(fp);

    TableCursor* cursor = start_table_scan(fp);
    TEST_ASSERT_NOT_NULL(cursor);

    stop_table_scan(&cursor);
    TEST_ASSERT_NULL(cursor);

    fclose(fp);
    remove("cursor_test.data");
}

void test_cursor_next_navigation(void) {
    FILE* fp = fopen("cursor_test.data", "wb+");
    
    // Define a simple schema
    ColumnDef def = make_column(TYPE_FIXED_STRING, 8);
    DbSchema* schema = alloc_schema(&def, 1);
    DbRow* rowIn = malloc_row(schema);

    // Manually setup a page with data to test navigation
    DbPage* page = create_page(0);
    
    // Prepare Record 1
    strcpy(rowIn->values[0].value.fixed_string, "Record1");
    size_t size1 = row_packed_size(schema, rowIn);
    void* buf1 = malloc(size1);
    pack_row(schema, rowIn, buf1);
    page_insert(page, buf1, size1);

    // Prepare Record 2
    strcpy(rowIn->values[0].value.fixed_string, "Record2");
    size_t size2 = row_packed_size(schema, rowIn);
    void* buf2 = malloc(size2);
    pack_row(schema, rowIn, buf2);
    page_insert(page, buf2, size2);
    
    writeback(page, fp);
    destroy_page(&page);
    free(buf1);
    free(buf2);

    // Test that cursor can navigate these records
    TableCursor* cursor = start_table_scan(fp);
    DbRow* rowOut = malloc_row(schema);

    // Record 1
    TEST_ASSERT_EQUAL_INT(1, cursor_next(cursor, schema, rowOut));
    TEST_ASSERT_EQUAL_STRING("Record1", rowOut->values[0].value.fixed_string);

    // Record 2
    TEST_ASSERT_EQUAL_INT(1, cursor_next(cursor, schema, rowOut));
    TEST_ASSERT_EQUAL_STRING("Record2", rowOut->values[0].value.fixed_string);

    // End of Table
    TEST_ASSERT_EQUAL_INT(0, cursor_next(cursor, schema, rowOut));

    dealloc_row(schema, rowIn);
    dealloc_row(schema, rowOut);
    dealloc_schema(schema);
    stop_table_scan(&cursor);
    fclose(fp);
    remove("cursor_test.data");
}
