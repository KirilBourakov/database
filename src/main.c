#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cursor.h"
#include "table.h"

int main(void) {
    const char* filename = "out.data";
    // Try to open for update, if fails, create with wb+
    FILE *fp = fopen(filename, "rb+");
    if (!fp) {
        fp = fopen(filename, "wb+");
    }
    
    if (!fp) {
        perror("Failed to open file");
        return 1;
    }

    ColumnDef defs[] = {
        make_column(TYPE_INT64),
        make_column(TYPE_FIXED_STRING, sizeof(char) * 4),
        make_column(TYPE_VAR_STRING, 100)
    };

    const DbSchema* schema = alloc_schema(defs, 3);

    // Initial scan to setup cursor
    TableCursor cursor = start_table_scan(fp);
    
    DbRow *current_row = malloc_row(schema);
    current_row->values[0].value.i = 32;
    strcpy(current_row->values[1].value.fixed_string, "2!");
    
    char* var_val = "Variable Data";
    current_row->values[2].value.var.bytes = strlen(var_val) + 1;
    current_row->values[2].value.var.data = malloc(current_row->values[2].value.var.bytes);
    memcpy(current_row->values[2].value.var.data, var_val, current_row->values[2].value.var.bytes);

    insert(&cursor, schema, current_row);

    writeback(&cursor.current_page, fp);
    dealloc_row(schema, current_row);

    // Scan from the beginning to print everything
    TableCursor scan_cursor = start_table_scan(fp);
    DbRow *rowOut = malloc_row(schema);
    while (cursor_next(&scan_cursor, schema, rowOut) != 0) {
        printf("%lld,%s,%s\n",
            rowOut->values[0].value.i,
            rowOut->values[1].value.fixed_string,
            rowOut->values[2].value.var.data
        );
    }
    dealloc_row(schema, rowOut);
    fclose(fp);
    dealloc_schema((DbSchema*)schema);
    return 0;
}
