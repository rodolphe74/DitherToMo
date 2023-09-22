#include <cmoc.h>
#include "tofloppy.h"
#include "lz77dec.h"

// set 160*200*16
void _160()
{
asm
{
    LDA #$7B
    STA $E7DC
}
}

// set palette
void _PALETTE(unsigned int *palette16)
{
asm {
    LDY :palette16
    CLRA
PALETTE_FOND
    PSHS A
    ASLA
    STA $E7DB
    LDD ,Y++       * couleur
    STB $E7DA
    STA $E7DA
    PULS A
    INCA
    CMPA #$10       * 16x
    BNE PALETTE_FOND
}
}


// switch to B plane
void _SWITCH_B()
{
asm {
    LDB    $E7C3
    ANDB   #$FE
    STB    $E7C3
}
}

void _WAITSPC()
{
asm {
TEST   JSR    $E806   * Keyboard routine
       TSTB           * Loop
       BEQ    TEST    * nothing
       CMPB   #$20    		* space
       BEQ    EXITKBDLOOP	* exit
       BRA    TEST    * next loop
EXITKBDLOOP
}
}


void loadAndDisplay(const char *name, const char *ext) {
    int l = sizeof(long);
    int sizex, sizexx, sizey;
    UCHAR aChunks, bChunks;
	
	// starting address of TO8 screen memory
    int *screenStart = (int *)0x4000;
    UCHAR *screenStartChar = (UCHAR *)screenStart;
	
	// 160x200x16 mode
	_160();
	
	// clean screen
	memset((UCHAR *) screenStartChar, 0, 8000);
	_SWITCH_B();
	memset((UCHAR *) screenStartChar, 0, 8000);

	// search File 
    FILE r;
    init();
    seekFile(name, ext, &r);

    UCHAR myBuf[2000];  // store what read from files

    // magick
    read(&r, myBuf, 3);
    if (!(myBuf[0]==0x01 && myBuf[1]==0x06 && myBuf[2]==0x07)) {
        printf("Wrong format![%d%d%d]%c%c", myBuf[0], myBuf[1], myBuf[2], 10, 13);
        exit(1);
    }

    // dimensions
    read(&r, myBuf, 1);
    sizex = myBuf[0];
    read(&r, myBuf, 1);
    sizexx = myBuf[0];
    read(&r, myBuf, 1);
    sizey = myBuf[0];
    printf("dimensions:(%d|%d)*%d%c%c", sizexx, sizex, sizey, 10, 13);

    // palette
    read(&r, myBuf, 32);
    _PALETTE((unsigned int *) myBuf);
    printf("palette loaded%c%c", 10, 13);

    // chunks
    read(&r, myBuf, 2);
    aChunks = myBuf[0];
    bChunks = myBuf[1];
    printf("chunks A count:%d%c%c", aChunks, 10, 13);
    printf("chunks B count:%d%c%c", bChunks, 10, 13);

    // init lz77 Dictionnary and window size (hardcoded in DitherToMo)
    initParameters(511, 9, 15, 4);

    // read chunk size
    UINT chunkSize = 0;
    UINT inputIdx = 0;
    UINT outputIdx = 0;
    UINT sz = 0;
    UCHAR count = 0;
    UCHAR isComp = 0;
    while (count < aChunks + bChunks) {
		// read loop
        read(&r, myBuf, 2);
        chunkSize = myBuf[0];
        chunkSize = chunkSize << 8;
        chunkSize |= myBuf[1];
//         printf("ChunkSize:%d (%d,%d) at %d%c%c", chunkSize, myBuf[0], myBuf[1], inputIdx, 10, 13);
        inputIdx +=2;

        read(&r, myBuf, 1);
        isComp = myBuf[0];
//         printf("Compressed (or not):%d%c%c", isComp, 10, 13);

        read(&r, myBuf, chunkSize);

        if(isComp) {
           sz = uncompress(myBuf, chunkSize - 1, (UCHAR *) (screenStartChar + outputIdx), 8000);
        } else {
            memcpy((UCHAR *) (screenStartChar + outputIdx), myBuf, chunkSize);
        }

        inputIdx += chunkSize;
        outputIdx += sz;
        count++;
        if (count == aChunks) {
            outputIdx = 0;
            _SWITCH_B();
        }
    }
}

int main(/*int argc, char *argv[]*/)
{
	loadAndDisplay("moby-d00", "167");
	_WAITSPC();
	loadAndDisplay("beast000", "167");
	_WAITSPC();
	loadAndDisplay("fouAPi00", "167");
	_WAITSPC();
	loadAndDisplay("arton400", "167");
	_WAITSPC();
	loadAndDisplay("arton500", "167");
	_WAITSPC();
	
    return 0;
}
