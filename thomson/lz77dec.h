#include <cmoc.h>
#define INT int
#define UINT unsigned int
#define UCHAR unsigned char
#define CHAR char


// Default search parameters
#define DIC_SIZE 4095
#define DIC_BIT_SIZE 12
#define AHEAD_SIZE 63
#define AHEAD_BIT_SIZE 6

#define CHAR_BIT_SIZE 8

#define MATCH_STRING_FUNC bruteForceSearchOptim



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

struct bitFieldStruct {
    UCHAR *buffer;
    UINT currentIndex;
    CHAR bitLeft;
};
typedef struct bitFieldStruct BitField;

void initDefaultParameters(void);
void initParameters(UINT dsz, UCHAR bdsz, UINT asz, UCHAR basz);
void initBitField(BitField *bf, UCHAR *buf);
INT bruteForceSearchOptim(UCHAR *x, INT m, UCHAR *y, INT n);

UINT readbits(BitField *bf, UCHAR bitCount);
INT uncompress(UCHAR *input, UINT iSize, UCHAR *output, UINT oSize);
