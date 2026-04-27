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

static inline int get_default_size(const DataType type) {
    static const int sizes[] = {
        #define X(name, size) size,
        DATATYPES(X)
        #undef X
    };
    return sizes[type];
}

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

static inline bool is_variable_size(DataType type) {
    return get_default_size(type) == -1;
}

#endif //DATABASE_DATATYPE_H