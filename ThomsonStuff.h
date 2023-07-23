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

using namespace std;

struct MAP_SEG_STRUCT {
    uint8_t columns;
    uint8_t lines;
    vector<uint8_t> rama;
    vector<uint8_t> ramb;
};
typedef MAP_SEG_STRUCT MAP_SEG;

static int thomson_level_pc[16] = { 0, 100, 127, 142, 163, 179, 191, 203, 215, 223, 231, 239, 243, 247, 251, 255 };
static float thomson_levels[16];
static int linear_to_to[256];
static int rgb_to_to[256];
static int thomson_level_magick[16] = { 0, 25600, 35512, 36352, 42728, 45824, 48896, 48896, 55040, 57088, 59136, 61184, 62208, 63232, 64256, 65535 };
static int rgb_to_to_comp[65536];

Color getColorForPaletteIndex(const map<string, PALETTE_ENTRY> &palette, int index);
void initThomsonCompensation();
int getPaletteThomsonValue(int r, int g, int b);
void save_map_40_col(const string &filename, const MAP_SEG &map_40, const map<string, PALETTE_ENTRY> &palette);
void save_map_40_col(const string &filename, const MAP_SEG &map_40, const map<string, PALETTE_ENTRY> &palette);
void convertBlocToThomson(uint8_t bloc[8], uint8_t thomson_bloc[3]);
void transposeDataMap40(int columns, int lines, const vector<uint8_t> &src, vector<uint8_t> &target);
int readAhead(const vector<uint8_t> &buffer_list, int idx);
void writeSegment(vector<uint8_t> &target, const vector<uint8_t> &buffer_list, int i, uint8_t seg_size);
void compress(vector<uint8_t> &target, FILE *f, const vector<uint8_t> &buffer_list, int enclose);
int getIndexColorThomsonTo(int back_index, int fore_index);
int getIndexColorThomsonMo(int back_index, int fore_index);
