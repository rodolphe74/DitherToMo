#pragma once
#include <stdio.h>
#include <stdint.h>
#define INT int16_t
#define UINT uint16_t
#define UCHAR unsigned char

#define F_BUFFER_SZ 512
#define C_F_BUFFER_SZ 512*2
#define U_F_BUFFER_SZ 512*2

// MAX 5 bits : 31
#define DIC_SIZE 31

// MAX 3 bits : 7
#define AHEAD_SIZE 7

struct tupleStruct {
    UCHAR d;
    UCHAR l;
    UCHAR c;
};
typedef struct tupleStruct Tuple;

struct emittedTupleStruct {
    UCHAR dl;
    UCHAR c;
};
typedef struct emittedTupleStruct EmittedTuple;

void toEmittedTuple(const Tuple *t, EmittedTuple *q);
void fromEmittedTuple(const EmittedTuple *q, Tuple *t);
INT compress(UCHAR *input, INT iSize, UCHAR *output, INT oSize);
INT uncompress(UCHAR *input, INT iSize, UCHAR *output, INT oSize);
void compressFile(FILE *fin, FILE *fout);
void uncompressFile(FILE *fin, FILE *fout);
