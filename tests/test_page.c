#include "../lib/unity/unity.h"
#include "../src/storage/page.h"
#include <string.h>
#include <stdlib.h>

void test_create_page(void) {
    int page_id = 42;
    DbPage* page = create_page(page_id);
    TEST_ASSERT_NOT_NULL(page);

    PageHeader* header = page_get_header(page);
    TEST_ASSERT_EQUAL_UINT32(page_id, header->page_id);
    TEST_ASSERT_EQUAL_UINT16(0, header->num_slots);
    TEST_ASSERT_EQUAL_UINT16(sizeof(PageHeader), header->slot_array_end);
    TEST_ASSERT_EQUAL_UINT16(PAGE_SIZE, header->row_data_start);

    destroy_page(&page);
}

void test_destroy_page(void) {
    DbPage* page = create_page(1);
    destroy_page(&page);
    TEST_ASSERT_NULL(page);
}

void test_page_insert(void) {
    DbPage* page = create_page(1);
    char* row = "Test Data";
    uint16_t size = strlen(row) + 1;

    int slot = page_insert(page, row, size);
    TEST_ASSERT_EQUAL_INT(0, slot);

    PageHeader* header = page_get_header(page);
    TEST_ASSERT_EQUAL_UINT16(1, header->num_slots);
    TEST_ASSERT_EQUAL_UINT16(sizeof(PageHeader) + PAGE_SLOT_SIZE, header->slot_array_end);
    TEST_ASSERT_EQUAL_UINT16(PAGE_SIZE - size, header->row_data_start);

    destroy_page(&page);
}

void test_slot_data(void) {
    DbPage* page = create_page(1);
    char* row = "Slot Test";
    uint16_t size = strlen(row) + 1;

    int slot = page_insert(page, row, size);
    void* data = slot_data(page, slot);

    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL_STRING(row, (char*)data);

    destroy_page(&page);
}

void test_page_full(void) {
    DbPage* page = create_page(1);

    // Max data we can fit is PAGE_SIZE - header - 1 slot
    uint16_t max_size = PAGE_SIZE - sizeof(PageHeader) - PAGE_SLOT_SIZE;
    void* large_row = malloc(max_size);
    memset(large_row, 'A', max_size);

    int slot = page_insert(page, large_row, max_size);
    TEST_ASSERT_EQUAL_INT(0, slot);

    // This should fail
    char* small_row = "Fail";
    int fail_slot = page_insert(page, small_row, 5);
    TEST_ASSERT_EQUAL_INT(-1, fail_slot);

    free(large_row);
    destroy_page(&page);
}
