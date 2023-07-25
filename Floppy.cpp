#include <iostream>
#include <fstream>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "Floppy.h"


using namespace std;

static void put_separator(char filename[])
{
    int last;

    last = strlen(filename) - 1;

    if (filename[last] != '/')
        filename[last + 1] = '/';
}


static int CreateEmptyArchive(const char sap_name[], int format, int capacity)
{
    sapID sap_file;

    if ((sap_file = sap_CreateArchive(sap_name, format)) == SAP_ERROR) {
        cerr << "Can't create sap image" << endl;
        return 1;
    }

    sap_FormatArchive(sap_file, capacity);
    sap_CloseArchive(sap_file);

    return 0;
}

static int AddFile(const char sap_name[], char *filename[], int nfiles)
{
    char pathname[FILENAME_LENGTH];
    int format, i, len = 0, ret = 0;
    struct stat s;
    struct dirent *entry;
    sapID sap_file;
    DIR *dir;

    if ((sap_file = sap_OpenArchive(sap_name, &format)) == SAP_ERROR) {
        cerr << "Can't open sap image" << endl;
        return 1;
    }

    for (i = 0; i < nfiles; i++) {
        /* get file statistics */
        if (stat(filename[i], &s) != 0) {
            cerr << "Can't find file " << filename[i]  << endl;
            ret = 1;
            break;
        }

        if (S_ISDIR(s.st_mode)) {  /* directory? */
            if ((dir = opendir(filename[i])) == NULL) {
                cerr << "Can't read directory " << filename[i]  << endl;
                ret = 1;
                break;
            }

            /* add every entry in turn */
            while ((entry = readdir(dir)) != NULL) {
                if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
                    strncpy(pathname, filename[i], FILENAME_LENGTH - 1);
                    put_separator(pathname);
                    strncat(pathname, entry->d_name, FILENAME_LENGTH - strlen(pathname) - 1);
                    len = sap_AddFile(sap_file, pathname);

                    if (len == 0) {
                        cerr << "pathname " << pathname << endl;
                        ret = 1;
                        break;
                    }
                }
            }

            closedir(dir);
        } else {
            len = sap_AddFile(sap_file, filename[i]);
        }

        if (len == 0) {
            if (ret == 0) {
                cerr << "filename " << filename[i] << endl;
                ret = 1;
            }
            break;
        }
    }

    sap_CloseArchive(sap_file);

    return ret;
}


// int writeFloppyWithBasLoaderOnDisk(std::string filename)
// {
//     ofstream out(filename.c_str(), ios::out | ios::binary);
//     if (!out) {
//         cout << "Cannot open file.";
//         return 1;
//     }
//
//     cout << "initializing sap floppy image:" << sizeof(floppy_sap) << endl;
//
//     out.write((char *) &floppy_sap, sizeof(floppy_sap));
//     out.close();
//     return 0;
// }


// AddFile(const char sap_name[], char *filename[], int nfiles)
int writeImagesListOnDisk(string floppyName, const string &filename, bool createArchive)
{
    int ret = 0;
    if (createArchive) {
        ret = CreateEmptyArchive(floppyName.c_str(), SAP_FORMAT1, SAP_TRK80);
        if (ret != 0) return ret;
    }
    char **filenamesArray = new char *[filename.size()];
    filenamesArray[0] = new char[255];
    strcpy(filenamesArray[0], filename.c_str());
    if (filename.size() == 0) return 0;
    ret = AddFile(floppyName.c_str(), filenamesArray, 1);
    if (ret != 0) return ret;

    delete[] filenamesArray[0];
    delete[] filenamesArray;

    return 0;
}

