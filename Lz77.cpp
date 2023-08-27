#include "lz77.h"
#include <string.h>
#include <stdlib.h>

void toEmittedTuple(const Tuple *t, EmittedTuple *q)
{
    q->dl = (t->d & 31) << 3;
    q->dl |= (t->l & 7);
    q->c  = t->c;
}

void fromEmittedTuple(const EmittedTuple *q, Tuple *t)
{
    t->d = (q->dl & 248) >> 3;
    t->l = q->dl & 7;
    t->c = q->c;
}

Tuple findInDic(UCHAR *input, INT startDicIndex, INT stopDicIndex, INT startAHead, UCHAR aHeadSize)
{

    if (startAHead == startDicIndex) {
        Tuple t = { 0, 0, input[startAHead] };
        return t;
    }

    Tuple t = {0, 0, input[startAHead]};
    UCHAR match = 0;
    INT i = 0, j = 0, k = 0;
    INT maxK = 0;
    while (startDicIndex < stopDicIndex) {
        // printf("*startDicIndex:%d   stopDicIndex:%d\n", startDicIndex, stopDicIndex);
        for (i = startAHead; i < startAHead + aHeadSize; i++) {
            match = 0;
            for (j = startDicIndex; j < stopDicIndex; j++) {    // TODO decrement
                if (input[j] == input[startAHead]) {
                    // printf("  ** match %d(%c)=%d(%c)\n", j, (char) input[j], startAHead, (char) input[startAHead]);
                    match = 1;
                    break;
                }
            }
            if (match) {
                k = 1;
                while (k < aHeadSize - 1 && input[j + k] == input[startAHead + k])
                    k++;

                // printf("  k=%d\n", k);
                if (k > maxK) {
                    t.d = (UCHAR) (i - j);   // WARNING
                    t.l = (UCHAR) k;         // WARNING
                    t.c = input[startAHead + k];    // TODO check overflow
                    maxK = k;
                    // printf("maxK:%d\n", maxK);
                }
                // startDicIndex += k;
                break;
            }
        }
        startDicIndex++;
    }
    return t;
}

INT compress(UCHAR *input, INT iSize, UCHAR *output, INT oSize)
{
    // printf("message size:%d\n", iSize);
    INT index = 0;
    INT dicIndexStart = 0, dicIndexStop = 0;
    // INT stopIndex = index + AHEAD_SIZE;
    INT outputIdx = 0;
    while (index < iSize) {

        dicIndexStart = (index /*- 1*/ -  DIC_SIZE) < 0 ? 0 : (index /*- 1*/ -  DIC_SIZE);
        dicIndexStop = index;

        // printf("index:%d  -  c:%c(%d)  -  dicIndexStart:%d   -  dicIndexStop:%d\n", index, (char)input[index], input[index], dicIndexStart, dicIndexStop);

        Tuple t = {0, 0, 0};
        t = findInDic(input, dicIndexStart, dicIndexStop, index, AHEAD_SIZE);
        // printf("  t=%d,%d,%c(%d)\n", t.d, t.l, (char) t.c, t.c);
        EmittedTuple q;
        toEmittedTuple(&t, &q);
        memcpy(output + outputIdx, &q, sizeof(q));
        outputIdx += sizeof(q);

        index += 1 + t.l;
    }
    return outputIdx;
}

INT uncompress(UCHAR *input, INT iSize, UCHAR *output, INT oSize)
{
    INT inputIdx = 0, outputIdx = 0;
    memset(output, 0, oSize);

    while (inputIdx < iSize) {
        // tuple t = {input[inputIdx], input[inputIdx + 1], input[inputIdx + 2]};
        EmittedTuple q = {input[inputIdx], input[inputIdx + 1]};
        Tuple t;
        fromEmittedTuple(&q, &t);

        if (t.d != 0) {
            memcpy(output + outputIdx, output + outputIdx - t.d, t.l);
            outputIdx += t.l;
        }

        output[outputIdx++] = t.c;
        // printf("buf:%s\n", output);

        // inputIdx += 3;
        // inputIdx += sizeof(tuple);
        inputIdx += sizeof(EmittedTuple);
    }
    return outputIdx;
}

void compressFile(FILE *fin, FILE *fout)
{
    UCHAR buffer[F_BUFFER_SZ];
    UCHAR cBuffer[C_F_BUFFER_SZ];
    memset(buffer, 0, sizeof(buffer));
    memset(cBuffer, 0, sizeof(cBuffer));
    UINT read = 0;
    INT csz = 0;
    while ((read = fread(buffer, 1, sizeof(buffer), fin)) > 0) {
        fwrite(&read, sizeof(UINT), 1, fout);
        printf("read:%d %lu\n", read, ftell(fout));
        csz = compress(buffer, read, cBuffer, C_F_BUFFER_SZ);
        printf("csz:%d >%c<  >%c<\n", csz, buffer[sizeof(buffer) - 1 ], cBuffer[csz - 1]);
        fwrite(cBuffer, csz, 1, fout);
        memset(buffer, 0, sizeof(buffer));
        memset(cBuffer, 0, sizeof(cBuffer));
    }
}

void uncompressFile(FILE *fin, FILE *fout)
{
    // UCHAR c[3];
    Tuple t;
    EmittedTuple q;
    UCHAR d[1];
    INT read = 0;
    // INT usz = 0;
    long cfp = 0;
    UINT fCount = 0;
    UINT fChunkSize = 0;

    while ((read = fread(&fChunkSize, sizeof(fChunkSize), 1, fin)) > 0) {
        // printf("chunk:%d %lu\n", fChunkSize, ftell(fin));
        fCount = 0;
        // while ((read = fread(&c, 1, sizeof(c), fin)) > 0) {
        // while ((read = fread(&t, sizeof(tuple), 1, fin)) > 0) {
        while ((read = fread(&q, sizeof(EmittedTuple), 1, fin)) > 0) {
            fromEmittedTuple(&q, &t);
            // printf("%lu read:%d\n", ftell(fin), read);
            // tuple t = {c[0], c[1], c[2]};
            // printf("%d,%d,%d   ", t.d, t.l, t.c);

            if (t.d != 0) {
                // memcpy(output + outputIdx, output + outputIdx - t.d, t.l);
                // outputIdx += t.l;
                cfp = ftell(fout);
                for (int i = 0; i < t.l; i++) {
                    fseek(fout, cfp - t.d + i, SEEK_SET);
                    fread(d, 1, sizeof(d), fout);
                    fseek(fout, 0, SEEK_END);
                    fwrite(d, 1, 1, fout);
                    fflush(fout);
                    fCount++;
                }
            }
            // printf("%d %lu - %c \n", fCount, cfp, (UCHAR) t.c);
            if (fCount < fChunkSize) {
                // when next car was taken out of message boundaries, no write
                fwrite(&t.c, 1, 1, fout);
                fflush(fout);
                fCount++;
            }

            if (fCount >= fChunkSize)
                break;
        }
    }
}
