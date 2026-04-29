//
// Created by Kiril on 4/26/2026.
//

#ifndef DATABASE_PAGE_H
#define DATABASE_PAGE_H

#define PAGE_SIZE 8192
#define PAGE_SLOT_SIZE sizeof(uint16_t)

#include <stdint.h>

typedef struct {
    uint32_t page_id;

    uint16_t slot_array_end;
    uint16_t row_data_start;
    uint16_t num_slots;
} PageHeader;

typedef struct DbPage DbPage;

/**
 * @brief Creates a new database page with the given ID.
 * 
 * @param id The ID of the page.
 * @return DbPage* A pointer to the newly created page.
 */
DbPage* create_page(int id);

/**
 * @brief Destroys a database page and deallocates its memory.
 * 
 * @param page_ptr A pointer to the page pointer to deallocate and nullify.
 */
void destroy_page(DbPage** page_ptr);

/**
 * @brief Retrieves the header of a database page.
 * 
 * @param page The page to query.
 * @return PageHeader* A pointer to the page's header.
 */
PageHeader* page_get_header(const DbPage* page);

/**
 * @brief Retrieves a pointer to the raw data segment of a page.
 * 
 * @param page The page to query.
 * @return void* A pointer to the raw data.
 */
void* page_get_raw_data(DbPage* page);

/**
 * @brief Retrieves a pointer to the data stored in a specific slot.
 * 
 * @param page The page to query.
 * @param slot The index of the slot to retrieve.
 * @return void* A pointer to the slot data.
 */
void* slot_data(DbPage* page, uint16_t slot);

/**
 * @brief Inserts a packed row into a page.
 * 
 * @param page The page to insert into.
 * @param packed_row The packed row data.
 * @param row_size The size of the packed row in bytes.
 * @return int 0 on success, non-zero if there is not enough space.
 */
int page_insert(DbPage* page, const void* packed_row, uint16_t row_size);

#endif //DATABASE_PAGE_H