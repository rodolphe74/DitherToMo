#pragma once

#include <string>
#include <map>
#include <vector>
#include <limits>
#include <cmath>
#include <iostream>
#include <string.h>
#include <stdint.h>
#include <Magick++.h>
#include "Structs.h"
#include "ColorStuff.h"

using namespace std;

struct MAP_SEG_STRUCT {
    uint8_t columns;
    uint8_t lines;
    vector<uint8_t> rama;
    vector<uint8_t> ramb;
};
typedef MAP_SEG_STRUCT MAP_SEG;

struct THOMSON_COLOR_STRUCT {
    int thomsonIndex;
    Color color;
};
typedef THOMSON_COLOR_STRUCT THOMSON_COLOR;

// 16bitColor = 8bitColor * 257
// 8bitColor = 16bitColor / 257

// Red : 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
static THOMSON_COLOR RED_255[16] = {
    {0, Color(0, 0, 0)},
    {1, Color(96, 0, 0)},
    {2, Color(122, 0, 0)},
    {3, Color(142, 0, 0)},
    {4, Color(158, 0, 0)},
    {5, Color(170, 0, 0)},
    {6, Color(183, 0, 0)},
    {7, Color(193, 0, 0)},
    {8, Color(204, 0, 0)},
    {9, Color(211, 0, 0)},
    {10, Color(219, 0, 0)},
    {11, Color(226, 0, 0)},
    {12, Color(234, 0, 0)},
    {13, Color(242, 0, 0)},
    {14, Color(249, 0, 0)},
    {15, Color(255, 0, 0)}
};

// Green : 0 16 32 48 64 80 96 112 128 144 160 176 192 208 224 240
static THOMSON_COLOR GREEN_255[16] = {
    {0, Color(0, 0, 0)},
    {16, Color(0, 96, 0)},
    {32, Color(0, 122, 0)},
    {48, Color(0, 142, 0)},
    {64, Color(0, 158, 0)},
    {80, Color(0, 170, 0)},
    {96, Color(0, 183, 0)},
    {112, Color(0, 193, 0)},
    {128, Color(0, 204, 0)},
    {144, Color(0, 211, 0)},
    {160, Color(0, 219, 0)},
    {176, Color(0, 226, 0)},
    {192, Color(0, 234, 0)},
    {208, Color(0, 242, 0)},
    {224, Color(0, 249, 0)},
    {240, Color(0, 255, 0)}
};

// Blue : 0 256 512 768 1024 1280 1536 1792 2048 2304 2560 2816 3072 3328 3584 3840
static THOMSON_COLOR BLUE_255[16] = {
    {0, Color(0, 0, 0)},
    {256, Color(0, 0, 96)},
    {512, Color(0, 0, 122)},
    {768, Color(0, 0, 142)},
    {1024, Color(0, 0, 158)},
    {1280, Color(0, 0, 170)},
    {1536, Color(0, 0, 183)},
    {1792, Color(0, 0, 193)},
    {2048, Color(0, 0, 204)},
    {2304, Color(0, 0, 211)},
    {2560, Color(0, 0, 219)},
    {2816, Color(0, 0, 226)},
    {3072, Color(0, 0, 234)},
    {3328, Color(0, 0, 242)},
    {3584, Color(0, 0, 249)},
    {3840, Color(0, 0, 255)}
};

static THOMSON_COLOR THOMSON_PALETTE[4096];

static int thomson_level_pc[16] = { 0, 100, 127, 142, 163, 179, 191, 203, 215, 223, 231, 239, 243, 247, 251, 255 };
static float thomson_levels[16];
static int linear_to_to[256];
static int rgb_to_to[256];
static int thomson_level_magick[16] = { 0, 25600, 35512, 36352, 42728, 45824, 48896, 48896, 55040, 57088, 59136, 61184, 62208, 63232, 64256, 65535 };
static int rgb_to_to_comp[65536];

void initThomsonPalette();
Image createThomsonImageFromRGBImage(const Image &rgbImage);
void removeExtraColorsFromThomsonPalette(const map<string, PALETTE_ENTRY> &palette, map<string, PALETTE_ENTRY> &thomsonPalette);
int createThomsonPaletteFromRGB(const map<string, PALETTE_ENTRY> &palette, map<string, PALETTE_ENTRY> &thomsonPalette);
void updateThomsonColorIndexes(map<string, PALETTE_ENTRY> &palette);
Image createImageFromThomsonPalette();
Color getColorForPaletteIndex(const map<string, PALETTE_ENTRY> &palette, int index);
uint16_t getThomsonIndexForPaletteIndex(const map<string, PALETTE_ENTRY> &palette, int index);
void initThomsonCompensation();
int getPaletteThomsonValue(int r, int g, int b);
void save_map_40_col(const string &filename, const MAP_SEG &map_40, const map<string, PALETTE_ENTRY> &palette);
void save_map_16(const string &filename, const MAP_SEG &map_16, int x_count, const map<string, PALETTE_ENTRY> &palette);
void streamMapC(ofstream &os, const string &name, const vector<uint8_t> &map, const string &suffix);
void convertBlocToThomson(uint8_t bloc[8], uint8_t thomson_bloc[3]);
void transposeDataMap40(int columns, int lines, const vector<uint8_t> &src, vector<uint8_t> &target);
int readAhead(const vector<uint8_t> &buffer_list, int idx);
void writeSegment(vector<uint8_t> &target, const vector<uint8_t> &buffer_list, int i, uint8_t seg_size);
void compress(vector<uint8_t> &target, const vector<uint8_t> &buffer_list, int enclose);
int getIndexColorThomsonTo(int back_index, int fore_index);
int getIndexColorThomsonMo(int back_index, int fore_index);
