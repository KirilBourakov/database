//
// Created by Kiril on 4/26/2026.
//

#ifndef DATABASE_DISK_H
#define DATABASE_DISK_H
#include <stdio.h>

#include "page.h"

int writeback(const DbPage* page, FILE* file);

#endif //DATABASE_DISK_H