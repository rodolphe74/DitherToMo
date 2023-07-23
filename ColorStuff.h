#pragma once

#include <string>
#include <map>
#include <vector>
#include <limits>
#include <cmath>
#include <iostream>
#include <stdint.h>
#include <string>
#include <Magick++.h>
#include "Structs.h"

using namespace std;

static char key[256];

string getPaletteKey(Color c);
float preciseColorDelta(Color c1, Color c2);
Color findPreciseClosestColorFromPalette(Color color, const map<string, PALETTE_ENTRY> &palette);
float correctedColorDelta(Color c1, Color c2);
Color findCorrectedClosestColorFromPalette(Color color, const map<string, PALETTE_ENTRY> &palette);
