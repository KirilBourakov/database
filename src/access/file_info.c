//
// Created by Kiril on 5/15/2026.
//
#include "file_info.h"

#include <stdio.h>
#include <stdlib.h>

#include "errors.h"
#include "storage/page.h"

#ifdef _WIN32
#define fseek_64 _fseeki64
#define ftell_64 _ftelli64
#else
#define fseek_64 fseeko
#define ftell_64 ftello
#endif

uint64_t get_page_count(FILE* fp) {
    int64_t current_pos = ftell_64(fp);

    fseek_64(fp, 0, SEEK_END);
    const int64_t size = ftell_64(fp);
    if (size % PAGE_SIZE) {
        DIE("size % PAGE_SIZE != 0; file corrupted.");
    }

    fseek_64(fp, current_pos, SEEK_SET);

    return (uint64_t)(size / PAGE_SIZE);
}
