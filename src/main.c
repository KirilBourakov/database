#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "access/cursor.h"
#include "model/row.h"
#include "storage/disk.h"

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
        make_column(TYPE_INT64, "pk"),
        make_column(TYPE_FIXED_STRING, sizeof(char) * 4, "val1"),
        make_column(TYPE_VAR_STRING, 100, "val2")
    };

    const DbSchema* schema = alloc_schema(defs, 3);

    // Initial scan to setup cursor
    TableCursor* cursor = start_table_scan(fp);

    DbRow *current_row = create_row(schema, &(int64_t){32}, "2! ", "V. Data", 8);

    insert(cursor, schema, current_row);

    writeback(cursor_get_page(cursor), fp);
    dealloc_row(schema, current_row);
    stop_table_scan(&cursor);

    // Scan from the beginning to print everything
    TableCursor* scan_cursor = start_table_scan(fp);
    DbRow *rowOut = malloc_row(schema);
    while (cursor_next(scan_cursor, schema, rowOut) != 0) {
        printf("%lld,%s,%s\n",
            rowOut->values[0].value.i,
            rowOut->values[1].value.fixed_string,
            rowOut->values[2].value.var.data
        );
    }
    dealloc_row(schema, rowOut);
    stop_table_scan(&scan_cursor);
    fclose(fp);
    dealloc_schema((DbSchema*)schema);
    return 0;
}
