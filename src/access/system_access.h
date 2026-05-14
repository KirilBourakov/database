//
// Created by KirilBourakov on 2026-05-14.
//

#ifndef SYSTEM_ACCESS_H
#define SYSTEM_ACCESS_H
#include "cursor.h"

typedef struct SystemAccessor SystemAccessor;

int insert_new_table(const SystemAccessor* sys_access, const DbSchema* schema);

#endif //SYSTEM_ACCESS_H
