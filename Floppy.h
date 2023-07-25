#pragma once
#include <string>
#include <vector>

using namespace std;

#define FILENAME_LENGTH 255

/* constants for the SAP physical file formats */
#define SAP_NSECTS      16

#define SAP_NTRACKS1    80
#define SAP_SECTSIZE1   256
#define SAP_TRACKSIZE1  (SAP_NSECTS*SAP_SECTSIZE1)

#define SAP_NTRACKS2    40
#define SAP_SECTSIZE2   128
#define SAP_TRACKSIZE2  (SAP_NSECTS*SAP_SECTSIZE2)


/* types used by the library */
typedef unsigned int sapID;

typedef struct {
   char format;
   char protection;
   char track;
   char sector;
   char data[258];
} sapsector_t;  /* type of a SAP sector */

typedef struct {
   int size;
   int file_type;
   int data_type;
   time_t date;
   char comment[9];
   int nblocks;
   int *block;
} sapfileinfo_t;

typedef int (*sapfilecb_t)(sapID id, const char filename[], int n, char trk20_data[]);


/* return values */
#define SAP_OK          0
#define SAP_NO_STD_FMT  (1<<0)
#define SAP_PROTECTED   (1<<1)
#define SAP_BAD_SECTOR  (1<<2)
#define SAP_CRC_ERROR   (1<<3)
#define SAP_ERROR       (1<<7)


/* errno values */
extern int sap_errno;

#define SAP_EBADF     1
#define SAP_EBUSY     2
#define SAP_EEMPTY    3
#define SAP_EFBIG     4
#define SAP_EINVAL    5
#define SAP_ENFILE    6
#define SAP_ENOENT    7
#define SAP_ENOSPC    8
#define SAP_EPERM     9
#define SAP_ETOOMANY  10


/* low-level functions */
extern int _ExtractDir(char buffer[], int buffer_size, int drive, int density, const char trk20_data[]);
extern int _ForEachFile(sapID id, const char pattern[], sapfilecb_t callback, int save_back);


/* physical format API functions */
#define SAP_FORMAT1  1
#define SAP_FORMAT2  2

extern "C" sapID sap_OpenArchive(const char filename[], int *format);
extern "C" sapID sap_CreateArchive(const char filename[], int format);
extern "C" int   sap_CloseArchive(sapID id);
extern "C" int   sap_FillArchive(sapID id, sapsector_t *sector);
extern "C" int   sap_ReadSector(sapID id, int track, int sect, sapsector_t *sector);
extern "C" int   sap_ReadSectorEx(sapID id, int track, int sect, int nsects, char data[]);
extern "C" int   sap_WriteSector(sapID id, int track, int sect, sapsector_t *sector);
extern "C" int   sap_WriteSectorEx(sapID id, int track, int sect, int nsects, const char data[]);


/* logical format API functions */
#define SAP_TRK40   1
#define SAP_TRK80   2

extern "C" int   sap_FormatArchive(sapID id, int capacity);
extern "C" int   sap_ListArchive(sapID id, char buffer[], int buffer_size);
extern "C" int   sap_AddFile(sapID id, const char filename[]);
extern "C" int   sap_DeleteFile(sapID id, const char pattern[]);
extern "C" int   sap_ExtractFile(sapID id, const char pattern[]);
extern "C" int   sap_GetFileInfo(sapID id, const char filename[], sapfileinfo_t *info);

// xxd -i floppy.sap > floppy.h


int writeImagesListOnDisk(string floppyName, const string &filename, bool createArchive);
