//
// Created by Kiril on 4/24/2026.
//

#ifndef DATABASE_DATATYPE_H
#define DATABASE_DATATYPE_H

#include <stdbool.h>

#define DATATYPES(X) \
    X(INT64,  8) \
    X(DOUBLE,  8) \
    X(FIXED_STRING, 0) \
    X(VAR_STRING, -1)

typedef enum {
    #define X(name, size) TYPE_##name,
        DATATYPES(X)
    #undef X
} DataType;

typedef enum {
    LITERAL = 0,
    FIXED_POINTER,
    VARIABLE_POINTER
} DataTypeClass;

/**
 * @brief Gets the default size in bytes for a given DataType.
 * 
 * @param type The DataType to query.
 * @return int The size in bytes, 0 for FIXED_STRING (needs explicit size), -1 for VAR_STRING.
 */
static inline int get_default_size(const DataType type) {
    static const int sizes[] = {
        #define X(name, size) size,
        DATATYPES(X)
        #undef X
    };
    return sizes[type];
}

/**
 * @brief Determines the DataTypeClass for a given DataType.
 * 
 * @param type The DataType to query.
 * @return DataTypeClass The class of the data type (LITERAL, FIXED_POINTER, or VARIABLE_POINTER).
 */
static inline DataTypeClass get_variable_typeclass(const DataType type) {
    switch (get_default_size(type)) {
        case -1:
            return VARIABLE_POINTER;
        case 0:
            return FIXED_POINTER;
        default:
            return LITERAL;
    }
}

/**
 * @brief Checks if a DataType is variable-sized.
 * 
 * @param type The DataType to check.
 * @return true if the type is VAR_STRING, false otherwise.
 */
static inline bool is_variable_size(DataType type) {
    return get_default_size(type) == -1;
}

#endif //DATABASE_DATATYPE_H