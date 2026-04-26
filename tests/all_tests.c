#include "../lib/unity/unity.h"
#include "../src/table.h"
#include "../src/page.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void setUp(void) {
    remove("test.data");
}

void tearDown(void) {
    remove("test.data");
}

// Table tests
void test_SingleRowWriteRead(void);
void test_MultipleRowsAppend(void);

// Page tests
void test_create_page(void);
void test_destroy_page(void);
void test_page_insert_success(void);
void test_page_insert_full(void);

int main(void) {
    UNITY_BEGIN();
    
    // Table Tests
    RUN_TEST(test_SingleRowWriteRead);
    RUN_TEST(test_MultipleRowsAppend);
    
    // Page Tests
    RUN_TEST(test_create_page);
    RUN_TEST(test_destroy_page);
    RUN_TEST(test_page_insert_success);
    RUN_TEST(test_page_insert_full);
    
    return UNITY_END();
}
