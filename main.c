#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int chunk_size_byte = 1024;

typedef struct {
    int x;
    int y;
} example_data;

long file_chunks(FILE *fp) {
    fseek(fp, 0, SEEK_END);
    const long file_size = ftell(fp);
    rewind(fp);
    if (file_size % chunk_size_byte != 0) {
        perror("File size should be multiple of chunk size");
    }
    return file_size / chunk_size_byte;
}

void read(FILE *fp) {
    const long chunks = file_chunks(fp);
    unsigned char *buffer = (unsigned char *)malloc(chunk_size_byte);
    for (int i = 0; i < chunks; i++) {
        fread(buffer, chunk_size_byte, 1, fp);
        example_data data;
        memcpy(&data, buffer, sizeof(data));
        printf("data.x = %d, data.y = %d\n", data.x, data.y);
    }
    free(buffer);
}

void write(FILE *fp, const example_data* data) {
    unsigned char *buffer = (unsigned char *)malloc(chunk_size_byte);
    memcpy(buffer, data, sizeof(*data));
    fwrite(buffer, 1, chunk_size_byte, fp);
    free(buffer);
}

int main(void) {
    const char* filename = "out.data";
    FILE *fp = fopen(filename, "ab+");
    const example_data data = { 1, 3 };
    write(fp, &data);
    read(fp);
}

