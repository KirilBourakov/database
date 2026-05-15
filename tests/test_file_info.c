#include "../lib/unity/unity.h"
#include "../src/access/file_info.h"
#include "../src/storage/page.h"
#include <stdio.h>
#include <stdlib.h>

void test_get_page_count_empty(void) {
    const char* filename = "test_empty.data";
    FILE* fp = fopen(filename, "wb+");
    TEST_ASSERT_NOT_NULL(fp);

    uint64_t count = get_page_count(fp);
    TEST_ASSERT_EQUAL_UINT64(0, count);

    fclose(fp);
    remove(filename);
}

void test_get_page_count_multiple_pages(void) {
    const char* filename = "test_pages.data";
    FILE* fp = fopen(filename, "wb+");
    TEST_ASSERT_NOT_NULL(fp);

    // Create 3 pages
    char buffer[PAGE_SIZE] = {0};
    for (int i = 0; i < 3; i++) {
        fwrite(buffer, 1, PAGE_SIZE, fp);
    }
    fflush(fp);

    uint64_t count = get_page_count(fp);
    TEST_ASSERT_EQUAL_UINT64(3, count);

    // Verify file pointer position is preserved
    fseek(fp, 10, SEEK_SET);
    get_page_count(fp);
    TEST_ASSERT_EQUAL_INT(10, ftell(fp));

    fclose(fp);
    remove(filename);
}
