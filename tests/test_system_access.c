#include "../lib/unity/unity.h"
#include "../src/access/system_access.h"
#include "../src/access/cursor.h"
#include "../src/storage/disk.h"
#include <stdio.h>
#include <stdlib.h>

void test_system_access_lifecycle(void) {
    const char* filename = "sys_access_test.data";
    FILE* fp = fopen(filename, "wb+");
    TEST_ASSERT_NOT_NULL(fp);

    init_db(fp);

    SystemAccessor* sys_access = alloc_system_access(fp);
    TEST_ASSERT_NOT_NULL(sys_access);

    dealloc_system_access(&sys_access);
    TEST_ASSERT_NULL(sys_access);

    fclose(fp);
    remove(filename);
}

void test_insert_and_get_schema(void) {
    const char* filename = "sys_access_test.data";
    FILE* fp = fopen(filename, "wb+");
    TEST_ASSERT_NOT_NULL(fp);

    init_db(fp);

    SystemAccessor* sys_access = alloc_system_access(fp);

    // Create a schema to insert
    ColumnDef cols[] = {
        make_column(TYPE_INT64, "id", COL_FLAG_PK),
        make_column(TYPE_VAR_STRING, 50, "name", COL_FLAG_NONE)
    };
    DbSchema* schema_in = alloc_schema(cols, 2);

    uint64_t root = insert_new_schema(sys_access, schema_in);
    TEST_ASSERT_EQUAL_UINT64(1, root); // Should be page 1 (since 0 is system)

    // Write back system page changes
    writeback(cursor_get_page(asTableAccess(sys_access)), fp);
    dealloc_system_access(&sys_access);

    // Re-open and check
    SystemAccessor* sys_access2 = alloc_system_access(fp);
    uint64_t loaded_root;
    DbSchema* schema_out = get_next_schema(sys_access2, &loaded_root);

    TEST_ASSERT_NOT_NULL(schema_out);
    TEST_ASSERT_EQUAL_UINT64(root, loaded_root);
    TEST_ASSERT_EQUAL_INT(2, schema_out->columns_count);
    TEST_ASSERT_EQUAL_STRING("id", schema_out->columns[0].name);
    TEST_ASSERT_EQUAL_STRING("name", schema_out->columns[1].name);

    dealloc_schema(schema_in);
    dealloc_schema(schema_out);
    dealloc_system_access(&sys_access2);

    fclose(fp);
    remove(filename);
}
