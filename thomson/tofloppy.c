#include <cmoc.h>
#include "tofloppy.h"


// globals
UCHAR *dkdrv = (UCHAR *) DKDRV;
int *dkbuf = (int *) DKBUF_BEGIN;
int *dktrk = (int *) DKTRK_BEGIN;
UCHAR *dksec = (UCHAR *) DKSEC;
UCHAR *dkopc = (UCHAR *) DKOPC;


UCHAR secBuf[256];
UCHAR *pbuf;
UCHAR blocks[256];
UINT curTrk;
UCHAR curSec;



void readSector(UINT trk, UCHAR sec)
{
    int *pSecBuf = (int *) &secBuf[0];
    asm {

        LDA #$00
        STA >DKDRV   // drive 0

        // 0x604a
        LDD :trk
        STD >DKTRK_BEGIN

        // 0x604c
        LDA :sec
        STA >DKSEC

        // 0x604f
        LDD :pSecBuf
        STD >DKBUF_BEGIN

        LDA #$02
        JSR DKCO
    }
    curTrk = trk;
    curSec = sec;
}





CHAR compareLong(LONG a, LONG b)
{
    UINT la = (a & 0xFFFF0000) >> 16;
    UINT lb = (b & 0xFFFF0000) >> 16;
    UINT ra = (a & 0x0000FFFF);
    UINT rb = (b & 0x0000FFFF);

    if (la > lb) return 1;
    if (lb > la) return -1;
    if (la == lb && ra > rb) return 1;
    if (la == lb && ra < rb) return -1;
    if (la == lb && ra == rb) return 0;
}


int seekFat(FILE *f, LONG fileByte, SEEK_FAT *sf)
{

    // // LONG size comp workaround
    // UINT la = (fileByte & 0xFFFF0000) >> 16;
    // UINT lb = (f->size & 0xFFFF0000) >> 16;
    // UINT ra = (fileByte & 0x0000FFFF);
    // UINT rb = (f->size & 0x0000FFFF);
    // if (la > lb)
    //     return -1;
    // if (la == lb && ra > rb)
    //     return -1;

    if (compareLong(fileByte, f->size) > 0)
        return -1;

    // size fit
    UCHAR blkIdx = (UCHAR) (fileByte / 2040L);
    UCHAR blk = f->blocks[blkIdx];
    UCHAR trk = blk / 2;
    UCHAR sec =  (UCHAR) (((UINT) (fileByte % 2040L)) / 255 + 1);
    UCHAR forw = (UCHAR) (fileByte % 255L);

    if (blk % 2)
        sec += 8;

    sf->blk = blk;
    sf->trk = trk;
    sf->sec = sec;
    sf->forw = forw;

    // printf("seekFat:%d %d %d %d%c%c", blk, trk, sec, forw, 10, 13);

    return 0;
}

INT read(FILE *f, UCHAR *where, UINT bytes)
{
    SEEK_FAT start;
    SEEK_FAT end;
    seekFat(f, f->idx, &start);
    LONG endIdx = f->idx + bytes;
    // printf("endIdx:%ld%c%c", endIdx,10,13);
    seekFat(f, f->idx + bytes, &end);

    readSector(start.trk, start.sec);
    UCHAR forwToNextSec = 0;
    UINT whereIdx = 0;

    while(1) {

        if (start.trk == end.trk && start.sec == end.sec && start.forw == end.forw) {
            // printf("f->idx updated=%ld > ", f->idx);
            f->idx += bytes;
            // printf("%ld%c%c", f->idx, 10, 13);
            break;
        }

        where[whereIdx++] = pbuf[start.forw++];
        if (whereIdx > bytes) {
            // debugSeekFat(&start);
            // debugSeekFat(&end);
            // printf("#widx:%d trk:%d sec:%d fw:%d%c%c", whereIdx, start.trk, start.sec, start.forw, 10, 13);

            // printf("f->idx updated=%ld > ", f->idx);
            f->idx += bytes;
            // printf("%ld%c%c", f->idx, 10, 13);

            break;
        }

        // if (start.forw == 255) {
        //     start.forw = 0;
        //     start.sec++;
        //     readSector(start.trk, start.sec);
        //     // printf("************ %d,%ld %d,%d #######%c%c", whereIdx, f->idx + whereIdx, start.trk, start.sec, 10, 13);
        //     if (start.trk == 36 && start.sec == 9) {
        //         for (int i = 0; i < 255; i++) {
        //             printf("%x ", pbuf[i]);
        //         }
        //         printf("------------%c%c", 10,13);
        //     }
        //
        // }
        if (start.forw == 255) {
            seekFat(f, f->idx + whereIdx, &start);
            readSector(start.trk, start.sec);
//             printf("************ %d,%ld %d,%d #######%c%c", whereIdx, f->idx + whereIdx, start.trk, start.sec, 10, 13);
            // if (start.trk == 36 && start.sec == 9) {
            //     for (int i = 0; i < 255; i++) {
            //         printf("%x ", pbuf[i]);
            //     }
            //     printf("------------%c%c", 10,13);
            // }

        }
        // if (start.sec > 16) {
        //     // printf("//%d%c%c", start.sec, 10, 13);
        //     start.sec = 1;
        //     // printf("//%d%c%c", start.sec, 10, 13);
        //     start.forw = 0;
        //     // start.trk++;    // PAS BON il faut se caler sur blk/trk/sec suivant
        //
        //     seekFat(f, f->idx + whereIdx, &start);  // ?
        //     readSector(start.trk, start.sec);
        // }
    }


    return whereIdx;
}



void feedBlocks(FILE *f)
{
    // empirical file size
    // 256 -> 0, 2, 1 -> 0*2040 + 1*255 + 1
    // 257 -> 0, 2, 2 -> 0*2040 + 1*255 + 2
    // 384 -> 0, 2, 129 -> 0*2040 + 1*255 + 129
    // 512 -> 0, 3, 2 -> 0*2040 + 2*255 + 2
    // 1024 -> 0, 5, 4 -> 0*2040 + 4*255 + 4
    // 6935 -> 3, 4, 50 -> 3*2040 + 3*255 + 50
    // 17708 -> 8, 6, 133 -> 8*2040 +5*255 + 113


    UCHAR blockCount = 0;
    UCHAR nextBlock =f->blocks[0];
    while (/*nextBlock >=0 &&*/ nextBlock <= 0xbf) {
        nextBlock = blocks[nextBlock + 1];  // F1=B0 - F1=B2 ... F160=B159
        blockCount++;
        f->blocks[blockCount] = nextBlock;
    }
    if (nextBlock >= 0xc1 && nextBlock <= 0xc8) {
        // last block (number of sectors used)
        f->lastSectors = nextBlock & 0x0f;
    }
    // compute size
    f->size = ((LONG)(blockCount - 1)) * 2040 + ((LONG)(f->lastSectors - 1)) * 255 + ((LONG)f->trkBytes);
    f->blockCount = blockCount;
}

void readBlocks()
{
    readSector(20, 2);
    memcpy(blocks, pbuf, 256);
}


void seekFile(const char name[8], const char ext[3], FILE *f)
{
    // iterate through fat catalogue until file found
#ifdef DEBUG_MODE
    printf("name size:%d%c%c", (int) strlen(name), 10, 13);
#endif
    f->idx = 0;
    for (UCHAR i = 3; i <= 16; i++) {
        readSector(20, i);
#ifdef DEBUG_MODE
        debugStatus();
        // debugSector();
#endif

        for (int j = 0; j < BYTES_PER_SEC; j += FILE_BYTES_SIZE) {
            if (*(pbuf + j) != 0xff) {
                if (strncmp((const char *) (pbuf + j), name, strlen(name)) == 0
                    && strncmp((const char *) (pbuf + j + 8), ext, strlen(ext)) == 0) {
                    f->blocks[0] = *(pbuf + j + 0x0d);
                    f->trkBytes = *(pbuf + j + 0x0e) * 256 + *(pbuf + j + 0x0f);
                    feedBlocks(f);
                    memset(f->name, 32, 8);
                    strncpy(f->name, name, strlen(name));
                    memset(f->ext, 32, 3);
                    strncpy(f->ext, ext, strlen(ext));
                    return;
                }
            }
        }
    }
}

void init()
{
    pbuf = (UCHAR *) secBuf;
    curTrk = 0;
    curSec = 0;
    readBlocks();
}

