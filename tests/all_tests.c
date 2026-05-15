#include "../lib/unity/unity.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void setUp(void) {}
void tearDown(void) {}

// Row tests
void test_malloc_dealloc_row(void);
void test_create_row(void);
void test_pack_unpack_row(void);
void test_row_contains(void);
void test_variable_limit_unlimited(void);
void test_variable_limit_enforced(void);

// Page tests
void test_create_page(void);
void test_destroy_page(void);
void test_page_insert(void);
void test_slot_data(void);
void test_page_full(void);

// Cursor tests
void test_cursor_lifecycle(void);
void test_cursor_next_navigation(void);

// File Info tests
void test_get_page_count_empty(void);
void test_get_page_count_multiple_pages(void);

// System Access tests
void test_system_access_lifecycle(void);
void test_insert_and_get_schema(void);

// Schema tests
void test_schema_serialization(void);

// Disk tests
void test_writeback_position(void);
void test_init_db(void);

int main(void) {
    UNITY_BEGIN();
    
    // Row Tests
    RUN_TEST(test_malloc_dealloc_row);
    RUN_TEST(test_create_row);
    RUN_TEST(test_pack_unpack_row);
    RUN_TEST(test_row_contains);
    RUN_TEST(test_variable_limit_unlimited);
    RUN_TEST(test_variable_limit_enforced);
    
    // Page Tests
    RUN_TEST(test_create_page);
    RUN_TEST(test_destroy_page);
    RUN_TEST(test_page_insert);
    RUN_TEST(test_slot_data);
    RUN_TEST(test_page_full);

    // Cursor Tests
    RUN_TEST(test_cursor_lifecycle);
    RUN_TEST(test_cursor_next_navigation);

    // File Info Tests
    RUN_TEST(test_get_page_count_empty);
    RUN_TEST(test_get_page_count_multiple_pages);

    // System Access Tests
    RUN_TEST(test_system_access_lifecycle);
    RUN_TEST(test_insert_and_get_schema);

    // Schema Tests
    RUN_TEST(test_schema_serialization);

    // Disk Tests
    RUN_TEST(test_writeback_position);
    RUN_TEST(test_init_db);
    
    return UNITY_END();
}
