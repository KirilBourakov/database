//
// Created by Kiril on 4/26/2026.
//

#ifndef DATABASE_DISK_H
#define DATABASE_DISK_H
#include <stdio.h>

#include "page.h"

/**
 * @brief Writes a page back to the disk.
 * 
 * @param page The page to write back.
 * @param file The file to write the page to.
 * @return int 0 on success, non-zero on failure.
 */
int writeback(const DbPage* page, FILE* file);

#endif //DATABASE_DISK_H