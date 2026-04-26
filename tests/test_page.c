#include "../lib/unity/unity.h"
#include "../src/page.h"
#include <string.h>
#include <stdlib.h>

void test_create_page(void) {
    int page_id = 42;
    DbPage* page = create_page(page_id);
    TEST_ASSERT_NOT_NULL(page);

    PageHeader* header = (PageHeader*)page->data;
    TEST_ASSERT_EQUAL_UINT32(page_id, header->page_id);
    TEST_ASSERT_EQUAL_UINT16(0, header->num_slots);
    TEST_ASSERT_EQUAL_UINT16(sizeof(PageHeader), header->slot_array_end);
    TEST_ASSERT_EQUAL_UINT16(PAGE_SIZE, header->row_data_start);

    free(page);
}

void test_destroy_page(void) {
    DbPage* page = create_page(1);
    destroy_page(&page);
    TEST_ASSERT_NULL(page);
}

void test_page_insert_success(void) {
    DbPage* page = create_page(1);
    char* row1 = "Hello";
    uint16_t size1 = strlen(row1) + 1;
    
    int id1 = page_insert(page, row1, size1);
    TEST_ASSERT_EQUAL_INT(0, id1);

    PageHeader* header = (PageHeader*)page->data;
    TEST_ASSERT_EQUAL_UINT16(1, header->num_slots);
    TEST_ASSERT_EQUAL_UINT16(sizeof(PageHeader) + PAGE_SLOT_SIZE, header->slot_array_end);
    TEST_ASSERT_EQUAL_UINT16(PAGE_SIZE - size1, header->row_data_start);
    TEST_ASSERT_EQUAL_MEMORY(row1, page->data + header->row_data_start, size1);

    char* row2 = "World!!!";
    uint16_t size2 = strlen(row2) + 1;
    int id2 = page_insert(page, row2, size2);
    TEST_ASSERT_EQUAL_INT(1, id2);

    TEST_ASSERT_EQUAL_UINT16(2, header->num_slots);
    TEST_ASSERT_EQUAL_UINT16(sizeof(PageHeader) + 2 * PAGE_SLOT_SIZE, header->slot_array_end);
    TEST_ASSERT_EQUAL_UINT16(PAGE_SIZE - size1 - size2, header->row_data_start);
    TEST_ASSERT_EQUAL_MEMORY(row2, page->data + header->row_data_start, size2);

    // Verify slots
    uint16_t offset1;
    memcpy(&offset1, page->data + sizeof(PageHeader), PAGE_SLOT_SIZE);
    TEST_ASSERT_EQUAL_UINT16(PAGE_SIZE - size1, offset1);

    uint16_t offset2;
    memcpy(&offset2, page->data + sizeof(PageHeader) + PAGE_SLOT_SIZE, PAGE_SLOT_SIZE);
    TEST_ASSERT_EQUAL_UINT16(PAGE_SIZE - size1 - size2, offset2);

    destroy_page(&page);
}

void test_page_insert_full(void) {
    DbPage* page = create_page(1);
    PageHeader* header = (PageHeader*)page->data;

    // Fill the page almost completely
    uint16_t large_row_size = PAGE_SIZE - sizeof(PageHeader) - PAGE_SLOT_SIZE;
    void* large_row = malloc(large_row_size);
    memset(large_row, 'A', large_row_size);

    int id = page_insert(page, large_row, large_row_size);
    TEST_ASSERT_EQUAL_INT(0, id);

    // Try to insert one more byte, should fail because we need space for slot too
    char small_row = 'B';
    int fail_id = page_insert(page, &small_row, 1);
    TEST_ASSERT_EQUAL_INT(-1, fail_id);

    free(large_row);
    destroy_page(&page);
}
