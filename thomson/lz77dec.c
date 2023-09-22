// #include <cmoc.h>
#include "lz77dec.h"


// Globals
UCHAR dicBitSize;
UINT dicSize;
UCHAR aheadBitSize;
UINT aheadSize;

void initDefaultParameters(void)
{
    dicSize = DIC_SIZE;
    dicBitSize = DIC_BIT_SIZE;
    aheadSize = AHEAD_SIZE;
    aheadBitSize = AHEAD_BIT_SIZE;
}

void initParameters(UINT dsz, UCHAR bdsz, UINT asz, UCHAR basz)
{
    dicSize = dsz;
    dicBitSize = bdsz;
    aheadSize = asz;
    aheadBitSize = basz;
}

void initBitField(BitField *bf, UCHAR *buf)
{
    bf->bitLeft = 7;
    bf->currentIndex = 0;
    bf->buffer = buf;
}


UINT readbits(BitField *bf, UCHAR bitCount)
{
    // little endian notation (last bit stored first)
    UCHAR mask = 0;
    UINT value = 0;
    UINT currentBit = 0;
    UCHAR bitSet = 0;
    while (bitCount) {
        mask = (UCHAR)(1 << bf->bitLeft);
        currentBit = (bf->buffer[bf->currentIndex] & mask) >> bf->bitLeft;
        value |= currentBit << bitSet;
        bitCount--;
        bitSet++;
        bf->bitLeft--;
        if (bf->bitLeft < 0) {
            bf->bitLeft = 7;
            bf->currentIndex++;
        }
    }
    return value;
}


// INT bruteForceSearchOptim(UCHAR *x, INT m, UCHAR *y, INT n)
// {
//     UCHAR *yb;
//     INT i = 0;
//
//     /* Searching */
//     for (yb = y; i <= n - m  ; ++y, i++) {
//         if (memcmp(x, y, m) == 0)
//             return y - yb;
//     }
//     return -1;
// }


INT uncompress(UCHAR *input, UINT iSize, UCHAR *output, UINT oSize)
{
    BitField bf;
    initBitField(&bf, input);
    INT /*inputIdx = 0,*/ outputIdx = 0;
    // memset(output, 0, oSize);

    while (bf.currentIndex < iSize) {

        Tuple t;
        t.d = readbits(&bf, dicBitSize);
        t.l = readbits(&bf, aheadBitSize);
        t.c = (UCHAR) readbits(&bf, CHAR_BIT_SIZE);

        if (t.d != 0) {
            memcpy(output + outputIdx, output + outputIdx - t.d, t.l);
            outputIdx += t.l;
        }

        output[outputIdx++] = t.c;
    }
    return outputIdx;
}
