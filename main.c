#include <stdio.h>
#include <string.h>

#include "table.h"

int main(void) {
    const char* filename = "out.data";
    FILE *fp = fopen(filename, "ab+");

    ColumnDef defs[] = {{TYPE_INT, sizeof(int64_t)}, {TYPE_FIXED_STRING, sizeof(char) * 4}};

    const DbSchema* schema = alloc_schema(defs,2);

    DbRow *row = malloc_row(schema);
    row->values[0].value.i = 32;
    strcpy(row->values[1].value.fixed_string, "2!");

    write_row(schema, fp, row);
    dealloc_row(schema, row);

    rewind(fp);

    DbRow *rowOut = malloc_row(schema);
    while (read_next_row(schema, fp, rowOut) > 0) {
        printf("%lld,%s\n", rowOut->values[0].value.i, rowOut->values[1].value.fixed_string);
    }
    dealloc_row(schema, rowOut);
}

