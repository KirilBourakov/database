#include <stdio.h>
#include "table.h"

int main(void) {
    const char* filename = "out.data";
    FILE *fp = fopen(filename, "ab+");

    const DbSchema schema = {2};
    DbRow *row = malloc_row(&schema);
    row->values[0].type = TYPE_INT;
    row->values[0].value.i = 32;
    row->values[1].type = TYPE_INT;
    row->values[1].value.i = 64;

    write_row(&schema, fp, row);
    dealloc_row(row);

    rewind(fp);

    DbRow *rowOut = malloc_row(&schema);
    while (read_next_row(&schema, fp, rowOut) > 0) {
        printf("%lld,%lld\n", rowOut->values[0].value.i, rowOut->values[1].value.i);
    }
    dealloc_row(rowOut);
}

