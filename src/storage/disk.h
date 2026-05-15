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

/**
 * Creates an empty page with id 0, and writes to database.
 * Used to actually create the database file.
 * @param fp File Pointer
 * @return
 */
void init_db(FILE* fp);

#endif //DATABASE_DISK_H