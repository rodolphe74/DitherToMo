#ifndef COMPILER_IS_CMOC
#pragma once
#include <stdio.h>
#include <stdint.h>
#define INT int16_t
#define UINT uint16_t
#define UCHAR unsigned char
#define CHAR char
#define INT32 int32_t
#define UINT32 uint32_t
#else
#include <cmoc.h>
#define INT int
#define UINT unsigned int
#define UCHAR unsigned char
#define CHAR char
#endif


// Size > DIC_SIZE + AHEAD_SIZE
#define F_BUFFER_SZ 16384
#define C_F_BUFFER_SZ F_BUFFER_SZ*3
#define U_F_BUFFER_SZ F_BUFFER_SZ*3

// Default search parameters
#define DIC_SIZE 4095
#define DIC_BIT_SIZE 12
#define AHEAD_SIZE 63
#define AHEAD_BIT_SIZE 6

#define CHAR_BIT_SIZE 8



#ifdef COMPILER_IS_CMOC
// even if all searches work on Thomson CMOC,
// try to avoid memory boyerMooreSearch
// & knuthMorrisPrattSearch memory overhead
#define MATCH_STRING_FUNC bruteForceSearchOptim
#else
#define MATCH_STRING_FUNC boyerMooreSearch
#endif



// global array used for knuthMorrisPratt search
// should be >= the size of window search
#ifndef COMPILER_IS_CMOC
#define KMP_LPS_SIZE 63
extern INT lps[KMP_LPS_SIZE];
#endif


// globals used for boyerMoore search
// bad suffix
#ifndef COMPILER_IS_CMOC
#define BM_XSIZE 256
// good suffix should be >= the size of window search
#define BM_ASIZE 63
#define MAX(a,b) (a>b?a:b)
extern INT bmBc[BM_XSIZE];
extern INT bmGs[BM_ASIZE];
#endif


// Global parameters
extern UINT dicSize;
extern UCHAR dicBitSize;
extern UINT aheadSize;
extern UCHAR aheadBitSize;


struct tupleStruct {
    UINT d;
    UINT l;
    UCHAR c;
};
typedef struct tupleStruct Tuple;

#ifndef COMPILER_IS_CMOC
struct FileStruct
{
    // struct used in file decompression
    FILE *f;
    int init;
    UCHAR currentChar;
};
typedef struct FileStruct File;

struct bitFieldStruct {
    // struct used on array compression/decompression (buffer)
    // or file decompression (struct file)
    union {
        UCHAR *buffer;
        File file;
    };
    UINT currentIndex;
    CHAR bitLeft;
};
#else
struct bitFieldStruct {
    UCHAR *buffer;
    UINT currentIndex;
    CHAR bitLeft;
};
#endif
typedef struct bitFieldStruct BitField;

void initDefaultParameters(void);
void initParameters(UINT dsz, UCHAR bdsz, UINT asz, UCHAR basz);
void initBitField(BitField *bf, UCHAR *buf);

// WARNING: search index on signed INT (16 bits INT : lz77 dictonary & window size < 32768)
// http://www-igm.univ-mlv.fr/~lecroq/string/index.html
INT bruteForceSearch(UCHAR *x, INT m, UCHAR *y, INT n);
INT bruteForceSearchOptim(UCHAR *x, INT m, UCHAR *y, INT n);
INT karpRabinSearch(UCHAR *x, INT m, UCHAR *y, INT n);
INT boyerMooreSearch(UCHAR *x, INT m, UCHAR *y, INT n);
// https://www.geeksforgeeks.org/kmp-algorithm-for-pattern-searching/
INT knuthMorrisPrattSearch(UCHAR *x, INT m, UCHAR *y, INT n);

Tuple findInDic(UCHAR *input, UINT inputSize,  UINT startDicIndex, UINT stopDicIndex, UINT startAHead, UINT aHeadSize);
void writebits(BitField *bf, UINT value, UCHAR bitCount);
UINT readbits(BitField *bf, UCHAR bitCount);
INT compress(UCHAR *input, UINT iSize, UCHAR *output, UINT oSize);
INT uncompress(UCHAR *input, UINT iSize, UCHAR *output, UINT oSize);
#ifndef COMPILER_IS_CMOC
void initBitFieldFile(BitField *bf, FILE *file);
UINT readbitsFile(BitField *bf, UCHAR bitCount, UINT *valueRead);
void finalizeWritebitsFile(BitField *bf, FILE *fout);
void compressFile(FILE *fin, FILE *fout);
void uncompressFile(FILE *fin, FILE *fout);
void uncompressFileUnbuffered(FILE *fin, FILE *fout);
#endif
