#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "access/cursor.h"
#include "access/system_access.h"
#include "model/row.h"
#include "storage/disk.h"

int main(void) {
    const char* filename = "out.data";
    FILE *fp = fopen(filename, "rb+");
    uint64_t user_table_root = 0;
    DbSchema* user_schema = NULL;

    if (!fp) {
        printf("Database file not found. Initializing new database...\n");
        // File does not exist, create it
        fp = fopen(filename, "wb+");
        if (!fp) {
            perror("Failed to create file");
            return 1;
        }
        init_db(fp);

        // Define the schema only when creating a new database
        ColumnDef defs[] = {
            make_column(TYPE_INT64, "pk", COL_FLAG_PK | COL_FLAG_NOT_NULLABLE),
            make_column(TYPE_FIXED_STRING, sizeof(char) * 4, "val1", COL_FLAG_NONE),
            make_column(TYPE_VAR_STRING, 100, "val2", COL_FLAG_NOT_NULLABLE)
        };
        user_schema = alloc_schema(defs, 3);

        // 2. Initialize System Access
        SystemAccessor* sys_access = alloc_system_access(fp);

        // 3. Insert user schema into system table
        user_table_root = insert_new_schema(sys_access, user_schema);
        
        // 4. Create the first page for the user table
        DbPage* user_page = alloc_page(user_table_root);
        writeback(user_page, fp);
        dealloc_page(&user_page);

        // 5. Commit changes to system table (write back page 0)
        writeback(cursor_get_page(asTableAccess(sys_access)), fp);
        dealloc_system_access(&sys_access);
        
        printf("Database initialized. Root page for user table: %llu\n", user_table_root);
    } else {
        printf("Database file found. Loading schema...\n");
        // File exists, load system info
        SystemAccessor* sys_access = alloc_system_access(fp);
        
        // Try to fetch the first (and only) schema in the system table
        user_schema = get_next_schema(sys_access, &user_table_root);
        
        if (!user_schema) {
            fprintf(stderr, "Failed to find any schema in the existing database file.\n");
            dealloc_system_access(&sys_access);
            fclose(fp);
            return 1;
        }
        
        dealloc_system_access(&sys_access);
        printf("Schema loaded. Root page for user table: %llu\n", user_table_root);
    }

    // Now that we have user_schema and user_table_root (either created or loaded), 
    // we can perform operations.

    // Insert a row into the user table
    TableCursor* cursor = alloc_table_cursor(fp, (int)user_table_root);
    int64_t pk = (int64_t)rand(); // Random PK for demonstration
    DbRow *current_row = alloc_filled_row(user_schema, &pk, "2! ", "New Variable Data Entry", (uint64_t)25);

    printf("Inserting row with PK: %lld\n", pk);
    insert(cursor, user_schema, current_row);
    writeback(cursor_get_page(cursor), fp);

    dealloc_row(user_schema, &current_row);
    dealloc_table_cursor(&cursor);

    // Scan and print all rows in the user table
    printf("Scanning user table contents:\n");
    TableCursor* scan_cursor = alloc_table_cursor(fp, (int)user_table_root);
    DbRow *rowOut = alloc_row(user_schema);
    int count = 0;
    while (cursor_next(scan_cursor, user_schema, rowOut)) {
        printf("Row %d: PK=%lld, val1='%s', val2='%s'\n",
            ++count,
            rowOut->values[0].value.i,
            rowOut->values[1].value.fixed_string,
            rowOut->values[2].value.var.data
        );
    }
    dealloc_row(user_schema, &rowOut);
    dealloc_table_cursor(&scan_cursor);
    
    fclose(fp);
    dealloc_schema(&user_schema);
    return 0;
}
