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

// Page tests
void test_create_page(void);
void test_destroy_page(void);
void test_page_insert(void);
void test_slot_data(void);
void test_page_full(void);

// Cursor tests
void test_cursor_lifecycle(void);
void test_cursor_next_navigation(void);

int main(void) {
    UNITY_BEGIN();
    
    // Row Tests
    RUN_TEST(test_malloc_dealloc_row);
    RUN_TEST(test_create_row);
    RUN_TEST(test_pack_unpack_row);
    RUN_TEST(test_row_contains);
    
    // Page Tests
    RUN_TEST(test_create_page);
    RUN_TEST(test_destroy_page);
    RUN_TEST(test_page_insert);
    RUN_TEST(test_slot_data);
    RUN_TEST(test_page_full);

    // Cursor Tests
    RUN_TEST(test_cursor_lifecycle);
    RUN_TEST(test_cursor_next_navigation);
    
    return UNITY_END();
}
