#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "table.h"

int main(void) {
    const char* filename = "out.data";
    FILE *fp = fopen(filename, "ab+");

    ColumnDef defs[] = {
        make_column(TYPE_INT64), 
        make_column(TYPE_FIXED_STRING, sizeof(char) * 4),
        make_column(TYPE_VAR_STRING, 100)
    };

    const DbSchema* schema = alloc_schema(defs, 3);

    DbRow *row = malloc_row(schema);
    row->values[0].value.i = 32;
    strcpy(row->values[1].value.fixed_string, "2!");
    
    char* var_val = "";
    row->values[2].value.var.bytes = strlen(var_val) + 1;
    row->values[2].value.var.data = malloc(row->values[2].value.var.bytes);
    memcpy(row->values[2].value.var.data, var_val, row->values[2].value.var.bytes);

    write_row(schema, fp, row);
    dealloc_row(schema, row);

    rewind(fp);

    DbRow *rowOut = malloc_row(schema);
    while (read_next_row(schema, fp, rowOut) > 0) {
        printf("%lld,%s,%s\n", 
            rowOut->values[0].value.i, 
            rowOut->values[1].value.fixed_string,
            rowOut->values[2].value.var.data);
    }
    dealloc_row(schema, rowOut);
    fclose(fp);
}
