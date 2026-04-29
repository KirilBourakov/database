//
// Created by Kiril on 4/13/2026.
//

#ifndef DATABASE_ROW_H
#define DATABASE_ROW_H
#include <stdint.h>
#include <stdio.h>

#include "data_type.h"
#include "schema.h"

typedef struct {
    DataType type;
    union {
        int64_t i;
        double f;
        char* fixed_string;
        struct {
            char* data;
            size_t bytes;
        } var;
    } value;
} DbValue;

typedef struct {
    DbValue* values;
} DbRow;

typedef struct {
    const char* column_name;
    const void* data;
    size_t bytes;
} MatchCondition;

/**
 * @brief Creates a new DbRow and populates it with values.
 * 
 * @param schema The schema defining the row structure.
 * @param ... Variadic arguments for the row values.
 * @return DbRow* A pointer to the newly created row.
 */
DbRow* create_row(const DbSchema* schema, ...);

/**
 * @brief Allocates memory for a DbRow based on the provided schema.
 * 
 * @param schema The schema defining the row structure.
 * @return DbRow* A pointer to the allocated row.
 */
DbRow* malloc_row(const DbSchema* schema);

/**
 * @brief Deallocates memory used by a DbRow.
 * 
 * @param schema The schema defining the row structure.
 * @param buffer The row to deallocate.
 */
void dealloc_row(const DbSchema* schema, DbRow* buffer);

/**
 * @brief Unpacks raw row memory into a DbRow structure.
 * 
 * @param schema The schema defining the row structure.
 * @param raw_row_memory The raw memory containing the packed row.
 * @param out_row The row structure to populate with unpacked data.
 */
void unpack_row(const DbSchema* schema, const void* raw_row_memory, const DbRow* out_row);

/**
 * @brief Packs a DbRow structure into a raw memory buffer.
 * 
 * @param schema The schema defining the row structure.
 * @param row The row to pack.
 * @param buffer The buffer to store the packed row.
 */
void pack_row(const DbSchema* schema, const DbRow* row, void* buffer);

/**
 * @brief Calculates the size of a DbRow when packed.
 * 
 * @param schema The schema defining the row structure.
 * @param row The row to measure.
 * @return size_t The size of the packed row in bytes.
 */
size_t row_packed_size(const DbSchema* schema, const DbRow* row);

/**
 * @brief Retrieves a pointer to a value in a row by column name.
 * 
 * @param schema The schema defining the row structure.
 * @param row The row to search.
 * @param name The name of the column.
 * @return DbValue* A pointer to the value, or NULL if not found.
 */
DbValue* get_row_value(const DbSchema* schema, const DbRow* row, const char* name);

/**
 * @brief Checks if a row matches a set of conditions.
 * 
 * @param schema The schema defining the row structure.
 * @param row The row to check.
 * @param conditions An array of match conditions.
 * @param count The number of conditions in the array.
 * @return true if the row matches all conditions, false otherwise.
 */
bool row_contains(const DbSchema* schema, const DbRow* row, const MatchCondition* conditions, size_t count);

#endif //DATABASE_ROW_H