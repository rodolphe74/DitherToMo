#pragma once

#include <Magick++.h>

using namespace Magick;

struct PALETTE_ENTRY_STRUCT {
    Color color;
    uint8_t index;
    uint16_t thomsonIndex;
};
typedef PALETTE_ENTRY_STRUCT PALETTE_ENTRY;

#define DITHER_SIZE 16
#define CLASH_SIZE 8
#define RESULT_SIZE_X 320.0
#define RESULT_SIZE_Y 200.0
#define LUMINOSITY_THRESHOLD 0.40
#define USE_MATRIX 9
