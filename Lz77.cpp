#include "lz77.h"
#ifndef COMPILER_IS_CMOC
#include <string.h>
#include <stdlib.h>
#else
#include <cmoc.h>
#endif

// Globals
UCHAR dicBitSize;
UINT dicSize;
UCHAR aheadBitSize;
UINT aheadSize;



void initDefaultParameters()
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

void writebits(BitField *bf, UINT value, UCHAR bitCount)
{
    // value must fit in bitCount !
    // little endian notation (last bit stored first)
    UCHAR bitSet = 0;
    while (value) {
        UCHAR currentBit = (UCHAR)(value & 1);
        bf->buffer[bf->currentIndex] |= currentBit << (bf->bitLeft);
        bf->bitLeft--;
        if (bf->bitLeft < 0) {
            bf->bitLeft = 7;
            bf->currentIndex++;
        }
        bitSet++;
        value = value >> 1;
    }
    for (UCHAR i = 0; i < bitCount - bitSet; i++) {
        bf->buffer[bf->currentIndex] |= (UCHAR)(0 << (bf->bitLeft));
        bf->bitLeft--;
        if (bf->bitLeft < 0) {
            bf->bitLeft = 7;
            bf->currentIndex++;
        }
    }
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

Tuple findInDic(UCHAR *input, UINT inputSize,  UINT startDicIndex, UINT stopDicIndex, UINT startAHead, UINT aHeadSize)
{
    if (startAHead == startDicIndex) {
        Tuple t = { 0, 0, input[startAHead] };
        return t;
    }

    Tuple t = {0, 0, input[startAHead]};
    UCHAR match = 0;
    INT i = 0, j = 0, k = 0;
    INT maxK = 0;
    while (startDicIndex <= stopDicIndex) {
        // printf("*startDicIndex:%d   stopDicIndex:%d\n", startDicIndex, stopDicIndex);
        for (i = startAHead; i < startAHead + aHeadSize; i++) {
            match = 0;
            for (j = startDicIndex; j <= stopDicIndex /*- 1*/; j++) {    // TODO decrement
                if (input[j] == input[startAHead]) {
                    // printf("  ** match %d(%c)=%d(%c)\n", j, (char) input[j], startAHead, (char) input[startAHead]);
                    match = 1;
                    break;
                }
            }
            if (match) {
                k = 1;
                while (k < aHeadSize - 1 && input[j + k] == input[startAHead + k]) {
                    if (j + k >= stopDicIndex) break;
                    k++;
                }

                if (k > maxK) {

                    if (startAHead + k > inputSize - 1) break;    // check overflow

                    t.d = (i - j);
                    t.l = k;
                    t.c = input[startAHead + k];
                    maxK = k;
                    // printf("maxK:%d %d %d\n", maxK, startAHead + k, startAHead);
                }
                break;
            }
        }
        startDicIndex++;
    }
    return t;
}

INT compress(UCHAR *input, UINT iSize, UCHAR *output, UINT oSize)
{
    // printf("iSize:%d   oSize:%d\n", iSize, oSize);
    memset(output, 0, oSize);   // uber wichtig !
    BitField bf;
    initBitField(&bf, output);

    INT index = 0;
    INT dicIndexStart = 0, dicIndexStop = 0;
    while (index < iSize) {

        dicIndexStart = (index /*- 1*/ -  dicSize) < 0 ? 0 : (index /*- 1*/ -  dicSize);
        dicIndexStop = index - 1;

        // printf("index:%d(%c)\n", index, input[index]);
        Tuple t = {0, 0, 0};
        t = findInDic(input, iSize, dicIndexStart, dicIndexStop, index, aheadSize);

        // printf("c %d  t=%d,%d,%c(%d)\n", iSize, t.d, t.l, (char) t.c, t.c);

        writebits(&bf, t.d, dicBitSize);
        writebits(&bf, t.l, aheadBitSize);
        writebits(&bf, t.c, CHAR_BIT_SIZE);

        index += 1 + t.l;
    }
    return bf.currentIndex;
}


INT uncompress(UCHAR *input, UINT iSize, UCHAR *output, UINT oSize)
{
    BitField bf;
    initBitField(&bf, input);
    INT /*inputIdx = 0,*/ outputIdx = 0;
    memset(output, 0, oSize);

    while (bf.currentIndex < iSize) {

        Tuple t;
        t.d = /*(UCHAR)*/ readbits(&bf, dicBitSize);
        t.l = /*(UCHAR)*/ readbits(&bf, aheadBitSize);
        t.c = (UCHAR) readbits(&bf, CHAR_BIT_SIZE);
        // printf("d  t=%d,%d,%c(%d)\n", t.d, t.l, (char) t.c, t.c);

        if (t.d != 0) {
            memcpy(output + outputIdx, output + outputIdx - t.d, t.l);
            outputIdx += t.l;
        }

        output[outputIdx++] = t.c;
    }
    return outputIdx;
}


#ifndef COMPILER_IS_CMOC
void initBitFieldFile(BitField *bf, FILE *file)
{
    bf->bitLeft = 7;
    bf->currentIndex = 0;
    bf->file.f = file;
    bf->file.init = 1;
}

UINT readbitsFile(BitField *bf, UCHAR bitCount, UINT *valueRead)
{
    // little endian notation (last bit stored first)
    UCHAR mask = 0;
    UINT value = 0;
    UINT currentBit = 0;
    UCHAR bitSet = 0;
    UINT o = 0;

    if (bf->file.init) {
        // o = fread(&currentChar, 1, 1, bf->file.f);
        o = fread(&bf->file.currentChar, 1, 1, bf->file.f);
        bf->file.init = 0;
    }
    while (bitCount) {
        mask = (UCHAR)(1 << bf->bitLeft);
        currentBit = (bf->file.currentChar & mask) >> bf->bitLeft;
        value |= currentBit << bitSet;
        *valueRead = value;
        bitCount--;
        bitSet++;
        bf->bitLeft--;
        if (bf->bitLeft < 0) {
            bf->bitLeft = 7;
            o = fread(&bf->file.currentChar, 1, 1, bf->file.f);
            if (!o) {
                return 0;
            }
        }
    }
    return 1;
}


void compressFile(FILE *fin, FILE *fout)
{
    // FILE *out = fopen("debug.txt", "wb");

    UCHAR buffer[F_BUFFER_SZ];
    UCHAR cBuffer[C_F_BUFFER_SZ];
    memset(buffer, 0, sizeof(buffer));
    memset(cBuffer, 0, sizeof(cBuffer));
    UINT read = 0;
    INT cidx = 0;
    INT csz = 0;
    while ((read = fread(buffer, 1, sizeof(buffer), fin)) > 0) {
        // printf("read:%d %lu\n", read, ftell(fout));

        cidx = compress(buffer, read, cBuffer, C_F_BUFFER_SZ);
        // printf("cidx:%d >%c(%d)<  >%c(%d)<\n", cidx, buffer[sizeof(buffer) - 1], buffer[sizeof(buffer) - 1], cBuffer[cidx - 1], cBuffer[cidx - 1]);

        // for (int i = 0; i < csz; i++) {
        //     printf("%d ", cBuffer[i]);
        // }
        // printf("\n");
        csz = cidx + 1;

        // fwrite(&csz, sizeof(UINT), 1, fout);    // write compressed chunk size
        fwrite(&csz, sizeof(UINT), 1, fout);    // write compressed chunk size

        // fwrite(cBuffer, csz + 1, 1, fout);          // write compressed buffer
        fwrite(cBuffer, csz, 1, fout);
        fflush(fout);

        // UCHAR unc[4096];
        // UINT uncSz = uncompress(cBuffer, cidx, unc, 4096);   // TODO it works because it can read csz+1 in cBuffer
        // // printf("uncompress sz:%d\n", uncSz);
        // fwrite(unc, uncSz, 1, out);

        memset(buffer, 0, sizeof(buffer));
        memset(cBuffer, 0, sizeof(cBuffer));
    }

    // fclose(out);
}

void finalizeWritebitsFile(BitField *bf, FILE *fout)
{
    // write, flush and close BitField buffer in file
    // size in computed with currentIndex + 1 (last bit index in bytes array).
    fwrite(bf->buffer, 1, bf->currentIndex + 1, fout);
    fclose(fout);
}


// **************************************
// Keep for later use.
// Uncompress without buffer.
// Use file structure from bitFieldStruct
// and seek directly on byte with fseek.
// Need a good system buffered disk read.
// Need to write uncompressed chunk size
// to synchronize (F_BUFFER_SZ).
// **************************************
// void uncompressFile(FILE *fin, FILE *fout)
// {
//     Tuple t;
//     EmittedTuple q;
//     UCHAR d[1];
//     INT read = 0;
//     long cfp = 0;
//     UINT fCount = 0;
//     UINT fChunkSize = 0;
//     UINT o = 1;
//     UINT valueRead = 0;
//     BitField bf;
//
//     while ((read = fread(&fChunkSize, sizeof(fChunkSize), 1, fin)) > 0) {
//         fCount = 0;
//         initBitFieldFile(&bf, fin);
//         printf("read:%d\n", fChunkSize);
//         while (o) {
//             o = readbitsFile(&bf, DIC_BIT_SIZE, &valueRead);
//             t.d = (UCHAR) valueRead;    // WARNING 255 max
//             o = readbitsFile(&bf, AHEAD_BIT_SIZE, &valueRead);
//             t.l = (UCHAR) valueRead;    // WARNING
//             o = readbitsFile(&bf, CHAR_BIT_SIZE, &valueRead);
//             t.c = (UCHAR) valueRead;    // WARNING
//
//             if (t.d != 0) {
//                 cfp = ftell(fout);
//                 for (int i = 0; i < t.l; i++) {
//                     fseek(fout, cfp - t.d + i, SEEK_SET);
//                     fread(d, 1, sizeof(d), fout);
//                     fseek(fout, 0, SEEK_END);
//                     fwrite(d, 1, 1, fout);
//                     fflush(fout);
//                     fCount++;
//                 }
//             }
//             if (fCount < fChunkSize) {
//                 // when next car was taken out of message boundaries, no write
//                 fwrite(&t.c, 1, 1, fout);
//                 fflush(fout);
//                 fCount++;
//             }
//
//             if (fCount >= fChunkSize) {
//                 fseek(fin, -1, SEEK_CUR);   // readbitsfile compensation TODO ca depend
//                 break;
//             }
//         }
//     }
// }


// ****************************************
// Buffered uncompression
// Read compressed chunk size.
// Read file compressed data on chunk size
// Uncompress current chunk with uncompress
// method
// Fast, simple and coherent.
// Require C_F_BUFFER_SZ + F_BUFFER_SZ bytes
// ****************************************
void uncompressFile(FILE *fin, FILE *fout)
{
    UCHAR cBuffer[C_F_BUFFER_SZ];
    UCHAR buffer[F_BUFFER_SZ];
    UINT chunkSize = 0;
    INT read = 0;
    UINT sz = 0;

    // printf("%lu %lu\n", sizeof(buffer), sizeof(cBuffer));

    memset(buffer, 0, sizeof(buffer));
    memset(cBuffer, 0, sizeof(cBuffer));

    while ((read = fread(&chunkSize, sizeof(UINT), 1, fin)) > 0) {
        read = fread(cBuffer, chunkSize, 1, fin);

        sz = uncompress(cBuffer, chunkSize - 1, buffer, F_BUFFER_SZ);
        fwrite(buffer, sz, 1, fout);

        memset(buffer, 0, sizeof(buffer));
        memset(cBuffer, 0, sizeof(cBuffer));
    }

    fflush(fout);
}

#endif
