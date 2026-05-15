#include "../lib/unity/unity.h"
#include "../src/storage/disk.h"
#include "../src/storage/page.h"
#include <stdio.h>
#include <stdlib.h>

void test_writeback_position(void) {
    const char* filename = "test_disk.data";
    FILE* fp = fopen(filename, "wb+");
    TEST_ASSERT_NOT_NULL(fp);

    // Create a page with ID 5
    DbPage* page = create_page(5);
    PageHeader* header = page_get_header(page);
    header->num_slots = 123; // Some dummy data

    writeback(page, fp);

    // Check file size
    fseek(fp, 0, SEEK_END);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(6 * PAGE_SIZE, ftell(fp));

    // Read back and verify
    fseek(fp, 5 * PAGE_SIZE, SEEK_SET);
    char buffer[PAGE_SIZE];
    TEST_ASSERT_EQUAL_INT(PAGE_SIZE, fread(buffer, 1, PAGE_SIZE, fp));

    PageHeader* read_header = (PageHeader*)buffer;
    TEST_ASSERT_EQUAL_UINT32(5, read_header->page_id);
    TEST_ASSERT_EQUAL_UINT16(123, read_header->num_slots);

    destroy_page(&page);
    fclose(fp);
    remove(filename);
}

void test_init_db(void) {
    const char* filename = "test_init.data";
    FILE* fp = fopen(filename, "wb+");
    TEST_ASSERT_NOT_NULL(fp);

    init_db(fp);

    // Check file size is exactly one page
    fseek(fp, 0, SEEK_END);
    TEST_ASSERT_EQUAL_INT(PAGE_SIZE, ftell(fp));

    // Verify page 0 content
    fseek(fp, 0, SEEK_SET);
    char buffer[PAGE_SIZE];
    fread(buffer, 1, PAGE_SIZE, fp);
    PageHeader* header = (PageHeader*)buffer;
    TEST_ASSERT_EQUAL_UINT32(0, header->page_id);

    fclose(fp);
    remove(filename);
}
