#include "../lib/unity/unity.h"
#include "../src/model/schema.h"
#include <stdlib.h>
#include <string.h>

void test_schema_serialization(void) {
    ColumnDef cols[] = {
        make_column(TYPE_INT64, "id", COL_FLAG_PK),
        make_column(TYPE_FIXED_STRING, 10, "code", COL_FLAG_NONE),
        make_column(TYPE_VAR_STRING, 100, "description", COL_FLAG_NOT_NULLABLE)
    };
    DbSchema* original = alloc_schema(cols, 3);

    size_t packed_size;
    void* packed = alloc_serialize_schema(original, &packed_size);
    TEST_ASSERT_NOT_NULL(packed);
    TEST_ASSERT_TRUE(packed_size > 0);

    DbSchema* unpacked = create_schema_from_packed((uint8_t*)packed);
    TEST_ASSERT_NOT_NULL(unpacked);
    TEST_ASSERT_EQUAL_INT(original->columns_count, unpacked->columns_count);

    for (int i = 0; i < 3; i++) {
        TEST_ASSERT_EQUAL_INT(original->columns[i].type, unpacked->columns[i].type);
        TEST_ASSERT_EQUAL_STRING(original->columns[i].name, unpacked->columns[i].name);
        TEST_ASSERT_EQUAL_INT(original->columns[i].bytes, unpacked->columns[i].bytes);
        TEST_ASSERT_EQUAL_INT(original->columns[i].flags, unpacked->columns[i].flags);
    }

    free(packed);
    dealloc_schema(original);
    dealloc_schema(unpacked);
}
