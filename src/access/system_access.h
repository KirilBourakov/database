//
// Created by KirilBourakov on 2026-05-14.
//

#ifndef SYSTEM_ACCESS_H
#define SYSTEM_ACCESS_H
#include "cursor.h"

typedef struct SystemAccessor SystemAccessor;

TableCursor* asTableAccess(const SystemAccessor* accessor);

/**
 * Inserts a new table schema into the database.
 * @param sys_access SystemAccessor object.
 * @param schema Pointer to the new schema.
 * @return The page number of the root page for the new schema
 */
uint64_t insert_new_schema(const SystemAccessor* sys_access, const DbSchema* schema);

/*
 * Fetch next schema in the database
 */
DbSchema* get_next_schema(const SystemAccessor* sys_access, uint64_t* schema_root);

SystemAccessor* alloc_system_access(FILE* file);
void dealloc_system_access(SystemAccessor** access);

#endif //SYSTEM_ACCESS_H
