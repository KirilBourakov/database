//
// Created by Kiril on 4/24/2026.
//

#ifndef DATABASE_DATATYPE_H
#define DATABASE_DATATYPE_H

#define DATATYPES(X) \
    X(INT64,  8) \
    X(DOUBLE,  8) \
    X(FIXED_STRING, 0)

typedef enum {
    #define X(name, size) TYPE_##name,
        DATATYPES(X)
    #undef X
} DataType;

inline int get_default_size(const DataType type) {
    static const int sizes[] = {
        #define X(name, size) size,
        DATATYPES(X)
        #undef X
    };
    return sizes[type];
}

#endif //DATABASE_DATATYPE_H