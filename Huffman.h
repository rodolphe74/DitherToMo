#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fstream>
#include <vector>

#define min(a, b) (a < b ? a : b)

typedef unsigned char BYTE;
typedef int16_t INT;
typedef uint16_t UINT;

// const INT search_buf    = 1024;
const INT search_buf    = 255;
const INT lookahead_buf = 100; // limited to 255

typedef struct tuple_struct {
    // unsigned short backward;
    BYTE backward;
    BYTE len;
    BYTE symbol;
} TUPLE;

typedef struct dynamic_list {
    TUPLE* buf;
    UINT    capacity;
    UINT    len;
} dynamic_list;

typedef struct decompressed_bytes {
    BYTE* buf;
    UINT   capacity;
    UINT   len;
} decompressed_bytes;

INT compression(BYTE buf[], INT len, const char* out);
INT compressionv(const std::vector<uint8_t> &in, std::vector<uint8_t> &out);
void decompress(BYTE* buf, UINT len, const char* out);
