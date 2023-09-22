#ifndef TO_FLOPPY
#define TO_FLOPPY

/*
 * 80 tracks of 16 secteurs
 * 1 sector = 256 bytes
 * 1 block = 8 sectors = 2048 bytes
 * 1 track = 2 blocks = 4096 bytes
 * 1 fat byte : 0xff = non allocated
 *              0xfe = reserved
 *              0x00 to 0xbf = next block number
 *              0xc1 to 0xc8 = sectors count in last block
 *
 * block number :   0
 *                  1 B0 - TRK0 - SEC1-8
 *                  2 B1 - TRK0 - SEC9-16
 *                  3 B2 - TRK1 - SEC8-8
 *                  4 B3 - TRK2 - SEC9-16
 *                  ...
 *                  160 B159 - TRK 79 - SEC9-16
 */


// Floppy
#define DKCO 0xe82a
#define DKOPC 0x6048
#define DKDRV 0x6049
#define DKSEC 0x604c
#define DKTRK_BEGIN 0x604a
#define DKTRK_END 0x604b
#define DKBUF_BEGIN 0x604f
#define DKBUF_END 0x6050
#define DKSTA 0x604e

#define BYTES_PER_SEC 256
#define FILES_PER_SEC 8
#define FILE_BYTES_SIZE 32
#define READ_SEC_BYTES 255


typedef unsigned int UINT;
typedef int INT;
typedef unsigned char UCHAR;
typedef char CHAR;
typedef unsigned long ULONG;
typedef long LONG;

extern UCHAR *dkdrv;
extern int *dkbuf;
extern int *dktrk;
extern UCHAR *dksec;
extern UCHAR *dkopc;

typedef struct FILE_STRUCT {
    char name[8];
    char ext[3];
    UCHAR blocks[160];
    UCHAR blockCount;
    UCHAR lastSectors;
    UINT trkBytes;
    LONG size;
    LONG idx;
} FILE;


typedef struct SEEK_FAT_STRUCT {
    UCHAR blk;
    UCHAR trk;
    UCHAR sec;
    UCHAR forw;
} SEEK_FAT;



// globals
extern UCHAR secBuf[256];
extern UCHAR *pbuf;
extern UCHAR blocks[256];
extern UINT curTrk;
extern UCHAR curSec;


// exported
void init();
void seekFile(const char name[8], const char ext[3], FILE *f);
INT read(FILE *f, UCHAR *where, UINT bytes);

#endif
